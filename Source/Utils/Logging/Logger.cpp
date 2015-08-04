#include "Logger.h"
#include <thread>
#include <chrono>
#include <iostream>

const std::string Logger::m_basePath = "Log/";
std::unordered_map<LogServiceId, std::unique_ptr<LoggingService, LoggingService::Deleter>> Logger::m_services;
std::unordered_map<std::string, LogServiceId> Logger::m_serviceIds;

LogServiceId Logger::m_errorLogId;
LogServiceId Logger::m_idCounter = 0;

std::thread Logger::m_logThread;
std::mutex Logger::m_logMutex;

std::atomic<bool> Logger::m_active = false;
std::vector<std::string> Logger::m_internalErrors;

void Logger::showErrors()
{
	for (auto& error : m_internalErrors)
		std::cout << error.c_str() << std::endl;

	m_internalErrors.clear();
}

void Logger::runOnThread()
{
	while (m_active || hasLogRequests())
	{
		// No need to waste cpu time for no reason
		for (int i = 0; i < 10 && m_active; ++i)
			std::this_thread::sleep_for(std::chrono::milliseconds(250));

		std::lock_guard<std::mutex> lock(m_logMutex);

		// Flip log request buffers if possible
		for (auto& it : m_services)
		{
			auto service = it.second.get();
			if (service->readyForFlip())
				service->flipRequestBuffers();
		}
	}

	// Stop all services
	for (auto& it : m_services)
	{
		auto service = it.second.get();
		service->setActive(false);
		service->getThread().join();
	}
}

bool Logger::hasLogRequests()
{
	for (auto& it : m_services)
		if (it.second->m_active && !it.second->m_writeBuffer->empty())
			return true;

	return false;
}

void Logger::run()
{
	if (!m_active)
	{
		m_active = true;
		m_errorLogId = requestService("Error.log");

		// Make sure the error log service is fine
		if (!m_services[m_errorLogId]->m_active)
		{
			m_internalErrors.push_back("Could not open 'Log/Error.log'. Make sure the Log folder exists.");
			m_active = false;
		}

		m_logThread = std::thread(runOnThread);
	}
}

void Logger::shutdown()
{
	m_active = false;
}

LogServiceId Logger::requestService(const std::string& filename)
{
	std::lock_guard<std::mutex> lock(m_logMutex);

	if (!m_active)
		return 0;

	if (m_serviceIds.count(filename))
	{
		LogServiceId id = m_serviceIds[filename];
		return id;
	}

	LogServiceId id = m_idCounter;
	
	m_serviceIds[filename] = id;
	m_services[id] = std::unique_ptr<LoggingService, LoggingService::Deleter>(new LoggingService(m_basePath + filename));

	m_services[id]->handleRequests();
	if (!m_services[id]->m_active && id != m_errorLogId)
		m_services[m_errorLogId]->log("Could not open file: " + filename);

	++m_idCounter;
	return id;
}

void Logger::log(const std::string& msg, LogServiceId serviceId)
{
	if (!m_active)
		return;

	std::lock_guard<std::mutex> lock(m_logMutex);

	if (m_services.count(serviceId))
	{
		LoggingService* service = m_services[serviceId].get();
		service->log(msg);
	}
}

void Logger::flipRequestBuffers(LoggingService* service)
{
	std::lock_guard<std::mutex> lock(m_logMutex);
	service->flipRequestBuffers();
}

void Logger::logError(const std::string& msg)
{
	log(msg, m_errorLogId);
}