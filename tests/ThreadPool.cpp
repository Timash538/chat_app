#include <ThreadPool.h>
#include <InterruptableThread.h>
#include <iostream>

ThreadPool::ThreadPool() :
	m_thread_count(std::thread::hardware_concurrency() != 0 ? std::thread::hardware_concurrency() : 4),
	m_thread_queues(m_thread_count),
	m_index(0)
{}

ThreadPool::~ThreadPool()
{
	for (auto& t : m_threads)
	{
		if(t)
		delete t;
	}
}

void ThreadPool::start() {
	for (int i = 0; i < m_thread_count; i++) {
		m_threads.emplace_back(new InterruptableThread( this, i));
	}
}
void ThreadPool::stop() {
	for (auto& t : m_threads) {
		t->interrupt();
	}
}
void ThreadPool::push_task(FuncType f, int id, int arg)
{
	int queue_to_push = m_index++ % m_thread_count;
	task_type task = [=] {f(id, arg); };
	m_thread_queues[queue_to_push].push(task);
}
void ThreadPool::threadFunc(int qindex)
{
	while (true)
	{
		if (InterruptableThread::checkInterrupted()) {
			cout << "thread was interrupted" << endl;
			return;
		}
		task_type task_to_do;
		bool res = false;
		int i = 0;
		for (; i < m_thread_count; i++)
		{
			if (res = m_thread_queues[(i + qindex) % m_thread_count].fastpop(task_to_do))
				break;
		}
		if (!res)
		{
			m_thread_queues[qindex].pop(task_to_do);
		}
		else if (!task_to_do)
		{
			m_thread_queues[(qindex + i) % m_thread_count].push(task_to_do);
		}
		if (!task_to_do)
		{
			return;
		}
		task_to_do();
	}
}