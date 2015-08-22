#include "LoggingService.h"
#include <fstream>
#include <iostream>
#include "Logger.h"
#include <sstream>
#include <ctime>

LoggingService::LoggingService(const std::string filename)
	:m_filename(filename), m_readBuffer(&m_requestBuffer1), m_writeBuffer(&m_requestBuffer2), m_readyForFlip(false), m_active(false), m_thread()
{
	std::ofstream file(filename, std::ios::out | std::ios::trunc);
	if (!file.is_open())
	file.close();
}

void LoggingService::log(const std::string& msg)
{
	time_t now = time(0);
	tm* d = std::localtime(&now);
	std::stringstream ss;
	ss << (d->tm_hour    < 10 ? "0" : "") << d->tm_hour    << ":" 
	   << (d->tm_min     < 10 ? "0" : "") << d->tm_min     << ":" 
	   << (d->tm_sec     < 10 ? "0" : "") << d->tm_sec     << " "
	   << (d->tm_mday    < 10 ? "0" : "") << d->tm_mday    << "."
	   << (1 + d->tm_mon < 10 ? "0" : "") << 1 + d->tm_mon << "."
	   << 1900 + d->tm_year << " - ";
	ss << msg;
	m_writeBuffer->push(ss.str());
}

void LoggingService::handleRequestsOnThread()
{
	std::ofstream file(m_filename, std::ios::out | std::ios::app);
	while (file.is_open() && (m_active || !m_readBuffer->empty()))
	{
		// No need to waste cpu time for no reason
		for (int i = 0; i < 10 && m_active; ++i)
			std::this_thread::sleep_for(std::chrono::milliseconds(250));

		while (!m_readBuffer->empty())
		{
			const std::string& msg = m_readBuffer->front();
			file << msg.c_str() << "\n";
			m_readBuffer->pop();
		}

		m_readyForFlip = true;

		// Just wait until the buffers are flipped
		while (m_active && m_readyForFlip)
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	file.close();
}

void LoggingService::handleRequests()
{
	if (!m_active)
	{
		// Check if the file can be opened
		std::ofstream file(m_filename, std::ios::out | std::ios::app);
		m_active = file.is_open();
		file.close();

		m_thread = std::thread(&LoggingService::handleRequestsOnThread, this);
	}
}

void LoggingService::flipRequestBuffers()
{
	std::swap(m_readBuffer, m_writeBuffer);
	m_readyForFlip = false;
}