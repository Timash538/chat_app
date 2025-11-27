#pragma once

#include <thread>
#include <functional>
#include <vector>
#include <BlockedQueue.h>
typedef std::function<void()> task_type;
typedef void (*FuncType) (int, int);

class InterruptableThread;

class ThreadPool
{
public:
	ThreadPool();
	void start();
	void interrupt();
	void push_task(FuncType f, int id, int arg);
	void threadFunc(int qindex);
private:
	std::vector<InterruptableThread*> m_threads;
	std::vector<BlockedQueue<task_type>> m_thread_queues;
	int m_thread_count;
	int m_index;
};