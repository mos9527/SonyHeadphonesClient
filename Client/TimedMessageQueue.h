#pragma once

#include <vector>
#include <string>
#include <deque>
#include <chrono>
#include <utility>
#include <limits>
#include <mutex>

using LargeCounter = unsigned long long;
using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;

auto constexpr DEFAULT_MAX_MESSAGES = 13371337;
constexpr auto DEFAULT_ERROR_MESSAGE_MS = 10000;

struct CommandMessage
{
public:
	CommandMessage(std::string message, TimePoint messageTime, TimePoint releaseTime)
	{
		this->message = std::move(message);
		this->_messageTime = messageTime;
		this->_releaseTime = releaseTime;
	}

	friend class TimedMessageQueue;

	std::string message;
	inline TimePoint messageTime() const { return _messageTime; }
	inline TimePoint releaseTime() const { return _releaseTime; }
private:
	TimePoint _messageTime;
	TimePoint _releaseTime;
};

class TimedMessageQueue
{
public:
	TimedMessageQueue(int maxMessages = DEFAULT_MAX_MESSAGES, int durationMs = DEFAULT_ERROR_MESSAGE_MS);

	//I: A message, and the length of time it should remain available
	void addMessage(std::string message);
	inline void iterateMessage(auto const& func) {
		std::scoped_lock lock(this->_msgMutex);
		for (auto it = begin(); it != end(); it++)
		{
			func(*it);
		}
	}
	inline const int size() const { return _messages.size(); }
private:
	//The iteators returned are guaranteed to stay valid only until the next `begin()` or `addMessage` call.
	std::deque<CommandMessage>::const_iterator begin();
	std::deque<CommandMessage>::const_iterator end() const;

	std::mutex _msgMutex;
	std::deque<CommandMessage> _messages;
	int _maxMessages = DEFAULT_MAX_MESSAGES;
	int _durationMs = DEFAULT_ERROR_MESSAGE_MS;
};