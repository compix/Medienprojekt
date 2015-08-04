#pragma once
#include <queue>
#include <atomic>
#include <thread>

class Logger;

/**
* Simple logging service. Using double buffering to ensure thread safety.
*/
class LoggingService
{
	friend Logger;

	// Required for std::unique_ptr because it's private
	struct Deleter
	{
		void operator()(LoggingService *service) const
		{
			delete service;
		}
	};
private:
	explicit LoggingService(const std::string filename);
	void log(const std::string& msg);

	void handleRequests();

	void flipRequestBuffers();
	inline bool readyForFlip() const { return m_readyForFlip; }
	inline void setActive(bool active) { m_active = active; }

	void handleRequestsOnThread();

	inline std::thread& getThread() { return m_thread; }
private:
	std::queue<std::string> m_requestBuffer1;
	std::queue<std::string> m_requestBuffer2;

	std::queue<std::string>* m_readBuffer;
	std::queue<std::string>* m_writeBuffer;

	std::string m_filename;

	std::atomic<bool> m_readyForFlip;
	std::atomic<bool> m_active;

	std::thread m_thread;
};