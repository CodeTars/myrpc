#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class LockQueue {
public:
    void Push(const T& value) {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(value);
        if (q.size() == 1) {
            cv.notify_one();
        }
    }

    T Pop() {
        std::unique_lock<std::mutex> lock(mtx);
        while (q.empty()) {
            cv.wait(lock);
        }
        T data = q.front();
        q.pop();
        return data;
    }

private:
    std::condition_variable cv;
    std::mutex mtx;
    std::queue<T> q;
};