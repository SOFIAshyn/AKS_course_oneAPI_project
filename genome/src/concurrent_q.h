//
// Created by heeve on 10.04.20.
//

#ifndef AC_ONET_CONCURRENT_Q_H
#define AC_ONET_CONCURRENT_Q_H

#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>

template <class T>
class concurrent_que{
    std::deque<T> que_m;
    mutable std::mutex m_m;
    std::condition_variable cv_m;

public:
    concurrent_que()= default;

    T back(){
        std::unique_lock<std::mutex> lg{m_m};
        cv_m.wait(lg, [this](){ return que_m.size() > 0; });
        return que_m.back();
    }

    T pop(){
        std::unique_lock<std::mutex> lg{m_m};
        cv_m.wait(lg, [this](){ return que_m.size() > 0; });
        T d = que_m.back();
        que_m.pop_back();
        return d;
    }

    void push(const T& v){

        std::unique_lock<std::mutex> lg{m_m};
        que_m.push_front(v);
        cv_m.notify_one();
    }

    size_t size() const { std::lock_guard<std::mutex> lg{m_m}; return que_m.size(); }
};


#endif //AC_ONET_CONCURRENT_Q_H
