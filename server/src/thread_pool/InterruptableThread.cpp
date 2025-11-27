#include <InterruptableThread.h>
#include <ThreadPool.h>

thread_local bool thread_interrupt_flag = false;

InterruptableThread::InterruptableThread(ThreadPool *pool, int qindex) : 
	m_pFlag(nullptr),
	m_thread(&InterruptableThread::startFunc, this, pool, qindex) {
}
InterruptableThread::~InterruptableThread()
{
	m_thread.join();
}
void InterruptableThread::interrupt()
{
		std::lock_guard<std::mutex> l(m_defender);
		if (m_pFlag)
			*m_pFlag = true;
}
void InterruptableThread::startFunc(ThreadPool *pool, int qindex)
{
	{
		std::lock_guard<std::mutex> l(m_defender);
		m_pFlag = &thread_interrupt_flag;
	}
	pool->threadFunc(qindex);
	{
		std::lock_guard<std::mutex> l(m_defender);
		m_pFlag = nullptr;
	}
}
bool InterruptableThread::checkInterrupted()
{
	return thread_interrupt_flag;
}