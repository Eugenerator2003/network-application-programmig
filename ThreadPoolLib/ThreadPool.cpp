#include "ThreadPool.h"

namespace Pool {

	ThreadPool::ThreadPool(int threadcount)
	{
		this->stoped = 0;
		this->count = threadcount;
		this->threads = std::vector<std::thread>(threadcount);

		for (int i = 0; i < threadcount; i++) {
			threads[i] = std::thread([this]() { this->equeueBody(); });
		}
	}

	void ThreadPool::enqueue(std::function<void()> task_f)
	{
		this->mtx.lock();
		tasks_f.push(task_f);
		this->mtx.unlock();
	}

	ThreadPool::~ThreadPool() {
		if (!this->deconstructed) {
			this->stop = true;
			for (int i = 0; i < this->count; i++) {
				this->threads[i].join();
			}
			this->deconstructed = true;
		}
	}

	void ThreadPool::equeueBody()
	{
		while (true) {
			if (this->stop) {
				break;
			}

			if (!this->tasks_f.empty()) {
				this->mtx.lock();
				if (!this->tasks_f.empty()) {
					std::function<void()> func = tasks_f.front();
					tasks_f.pop();
					this->mtx.unlock();
					func();
				}
				else {
					this->mtx.unlock();
				}
			}
		}
		this->stoped++;
	}
}
