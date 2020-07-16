#pragma once

#include <queue>
#include <mutex>

template<typename T>
class CQueue
{
public:
	~CQueue()
	{
		std::lock_guard<std::mutex> lock(m);
		std::queue<T> empty;
		que.swap(empty);
	}
	int push(T ele)
	{
		std::lock_guard<std::mutex> lock(m);
		que.push(ele);
		return 0;
	}
	int pop()
	{
		std::lock_guard<std::mutex> lock(m);
		que.pop();
		return 0;
	}
	int pop(T& ele)
	{
		std::lock_guard<std::mutex> lock(m);
		if (que.size() > 0)
		{
			ele = que.front();
			que.pop();
		}
		return 0;
	}
	int size()
	{
		std::lock_guard<std::mutex> lock(m);
		int _size = que.size();
		return _size;
	}
	void clear()
	{
		std::lock_guard<std::mutex> lock(m);
		std::queue<T> empty;
		que.swap(empty);
	}
private:
	std::queue<T> que;
	std::mutex m;
};

