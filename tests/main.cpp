#include <ThreadPool.h>
#include <InterruptableThread.h>
#include <iostream>

using namespace std;

mutex coutLocker;

void task(int id, int delay)
{
	for (int i = 0; i < delay; i++) {
		if (InterruptableThread::checkInterrupted()) {
			unique_lock<mutex> l(coutLocker);
			cout << "task " << id << " was interrupted" << endl;
			return;
		}
		this_thread::sleep_for(chrono::seconds(1));
	}
	unique_lock<mutex> l(coutLocker);
	cout << "task " << id << " made by thread_id " << this_thread::get_id() << " this took " << delay << " seconds" << endl;
}

int main()
{
	ThreadPool t_pool;
	t_pool.start();
	for (int i =0; i<100; i++)
	{
		t_pool.push_task(task,i,1 + rand() %2);
	}
	this_thread::sleep_for(chrono::seconds(4));
	t_pool.stop();
	
}