#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

class Barrier {
private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
    int originalCount;

public:
    Barrier(int n) : count(n), originalCount(n) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        if (--count == 0) {
            count = originalCount;
            cv.notify_all();
        } else {
            cv.wait(lock, [this] { return count == originalCount; });
        }
    }
};