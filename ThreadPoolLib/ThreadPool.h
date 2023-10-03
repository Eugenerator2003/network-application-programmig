#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <functional>

namespace Pool {

	class ThreadPool
	{
	public:
		ThreadPool(int threadcount);
		void enqueue(std::function<void()> task_f);
		~ThreadPool();
	private:
		int count;
		int stoped;
		bool stop{ false };
		std::mutex mtx;
		std::condition_variable condition;
		std::vector<std::thread> threads;
		std::queue<std::function<void()>> tasks_f;

		bool deconstructed;

		virtual void equeueBody();
	};
}

