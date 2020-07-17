#pragma once

#include <mutex>
#include <condition_variable>

class condition_wrapper
{
public:
	void wait();
	void notify_one();
	void notify_all();
private:
	std::condition_variable condition;
	std::mutex mut;
};

