#pragma once

#include <thread>
#include <functional>
#include <vector>
#include <server/BlockedQueue.h>
typedef std::function<void()> task_type;
typedef void (*FuncType) (std::string);

class InterruptableThread;

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();
	void start();
	void stop();
	void push_task(task_type);
	void threadFunc(int qindex);
private:
	int m_thread_count = 0;
	int m_index = 0;
	std::vector<InterruptableThread*> m_threads;
	std::vector<BlockedQueue<task_type>> m_thread_queues;
	
};