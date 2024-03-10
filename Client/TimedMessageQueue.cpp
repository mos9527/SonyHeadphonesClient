#include "TimedMessageQueue.h"

TimedMessageQueue::TimedMessageQueue(int maxMessages, int durationMs)
{
	this->_maxMessages = maxMessages;
	this->_durationMs = durationMs;	
}

void TimedMessageQueue::addMessage(std::string message)
{
	std::scoped_lock lock(this->_msgMutex);

	if (this->_maxMessages >= 0 && this->_messages.size() == this->_maxMessages)
	{
		this->_messages.pop_front();
	}

	this->_messages.emplace_back(std::move(message), Clock::now(), Clock::now() + std::chrono::milliseconds(this->_durationMs));
}


std::deque<CommandMessage>::const_iterator TimedMessageQueue::begin()
{
	while (this->_messages.size() > 0)
	{
		if (this->_durationMs >= 0 && this->_messages.front()._releaseTime < Clock::now())
		{
			this->_messages.pop_front();
		}
		else 
		{
			//break if we reached a non-stale message, to improve perf
			break;
		}
	}

	return this->_messages.cbegin();
}

std::deque<CommandMessage>::const_iterator TimedMessageQueue::end() const
{
	return this->_messages.cend();
}
