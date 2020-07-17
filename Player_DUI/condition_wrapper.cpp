#include "pch.h"
#include "condition_wrapper.h"

void condition_wrapper::wait()
{
	std::unique_lock<std::mutex> lk(mut);
	condition.wait(lk);
}

void condition_wrapper::notify_one()
{
	condition.notify_one();
}

void condition_wrapper::notify_all()
{
	condition.notify_all();
}

