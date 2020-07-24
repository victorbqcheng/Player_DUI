#pragma once

#include <deque>
#include <queue>
#include <mutex>
#include <functional>

template<typename T>
class CQueue
{
	using inner_que = std::queue<T>;
public:
	~CQueue()
	{
		std::lock_guard<std::mutex> lock(m);
		inner_que empty;
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
		inner_que empty;
		que.swap(empty);
	}
private:
	inner_que que;
	std::mutex m;
};


template<typename T>
class CDeque
{
	using inner_deque = std::deque<T>;
public:
	~CDeque()
	{
		clear();
	}
	int push_back(T& t)
	{
		std::lock_guard<std::mutex> lock(m);
		que.push_back(t);
		return 0;
	}
	bool pop_back(T& t)
	{
		std::lock_guard<std::mutex> lock(m);
		if (que.size() > 0)
		{
			t = que.back();
			que.pop_back();
			return true;
		}
		else
		{
			return 0;
		}
	}
	int push_front(T& t)
	{
		std::lock_guard<std::mutex> lock(m);
		que.push_front(t);
		return 0;
	}
	bool pop_front(T& t)
	{
		std::lock_guard<std::mutex> lock(m);
		if (que.size() > 0)
		{
			t = que.front();
			que.pop_front();
			return true;
		}
		else
		{
			return 0;
		}
	}
	bool front(T& t)
	{
		std::lock_guard<std::mutex> lock(m);
		if (que.size() > 0)
		{
			t = que.front();
			return true;
		}
		else
			return false;
	}
	bool back(T&)
	{
		std::lock_guard<std::mutex> lock(m);
		if (que.size() > 0)
		{
			t = que.back();
			return true;
		}
		else
			return false;
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
		inner_deque empty;
		que.swap(empty);
	}
	bool find(T& t, std::function<bool(T const& t)> pred)
	{
		std::lock_guard<std::mutex> lock(m);
		if(std::find_if(que.begin(), que.end(), pred) == que.end())
			return false;
		return true;
	}
private:
	inner_deque que;
	std::mutex m;
};
