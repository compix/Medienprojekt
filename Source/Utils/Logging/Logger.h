#pragma once
#include <vector>
#include "LoggingService.h"
#include <unordered_map>
#include <cinttypes>
#include <memory>
#include <mutex>
#include <thread>

typedef uint32_t LogServiceId;

/**
* Simple logger class to spam your .log files with "important" stuff.
*
* Request a logging service ID with the specified filename, save it and use it for logs.
*
* Example:
* LogServiceId serviceId = Logger::requestService("VeryImportant.log");
* Logger::log("Some cool event just happened and idk why.", serviceId);
*/
class Logger
{
public:
	static void run();
	static void shutdown();
	static LogServiceId requestService(const std::string& filename);

	static void log(const std::string& msg, LogServiceId serviceId);
	static void logError(const std::string& msg);

	static inline const std::vector<std::string>& getErrors() { return m_internalErrors; }

	static inline std::thread& getLogThread() { return m_logThread; }

	static inline bool hasErrors() { return m_internalErrors.size() > 0; }
	static void showErrors();
private:
	static void runOnThread();

	static bool hasLogRequests();
	static void flipRequestBuffers(LoggingService* service);
private:
	static std::unordered_map<LogServiceId, std::unique_ptr<LoggingService, LoggingService::Deleter>> m_services;
	static std::unordered_map<std::string, LogServiceId> m_serviceIds;
	static const std::string m_basePath;
	static LogServiceId m_errorLogId;

	static LogServiceId m_idCounter;

	static std::mutex m_logMutex;

	static std::atomic<bool> m_active;

	static std::vector<std::string> m_internalErrors;

	static std::thread m_logThread;
};