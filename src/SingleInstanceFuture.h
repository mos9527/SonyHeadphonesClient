#pragma once

#include <future>
#include <chrono>
#include <functional>
#include <utility>

//A simple future wrapper class. Allows only a single instance to be active at any single time; particularly useful for GUI operations where we don't want
//to send the same commands or repeat operations before they completed. Also wraps std::async nicely with setFromAsync
template <class T>
class SingleInstanceFuture : public std::future<T>
{
public:
    std::string name = "<unamed>";
	explicit SingleInstanceFuture(std::string const& name) : name(name) {};
    ~SingleInstanceFuture() {
        if (this->valid() && !this->ready())
        {
            this->wait();
        }
    }
    inline void setFromAsync(auto func, auto && ...args)
    {
        if (this->valid())
        {
            throw std::runtime_error("The asynchronous action was cancelled before it finished executing");
        }
        std::future<T>::operator=(std::async(std::launch::async, func, std::forward<decltype(args)>(args)...));
    }
    inline bool ready()
    {
        if (!this->valid())
        {
            return false;
        }
        return this->wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }
    inline void reset(){
        std::future<T>::operator=({});
    }
};
