#pragma once
#include <thread>
#include <mutex>
#include <chrono>

class ThreadPool;

extern thread_local bool thread_interrupt_flag;

class InterruptableThread
{
public:
	InterruptableThread(ThreadPool* pool, int qindex);
	~InterruptableThread();
	void interrupt();
	inline void startFunc(ThreadPool* pool, int qindex);

	static bool checkInterrupted();

private:
	bool* m_pFlag;
	std::mutex m_defender;
	std::thread m_thread;
};
