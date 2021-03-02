//
// Created by Endré Hadzalic on 02.03.2021.
//

#ifndef OVING4_WORKERS_H
#define OVING4_WORKERS_H

#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <atomic>

using namespace std;


class Workers {
private:
    static list<function<void()>> functionQueue;
    static vector<thread> threads;
    static mutex taskMutex;
    static condition_variable cv;
    static atomic<bool> running;
    static int amountOfThreads;

public:
    /**
     *
     * @param threadAmount
     */
    Workers(int threadAmount) {
        amountOfThreads = threadAmount;
        running = false;
    }

    /**
     *
     */
    void start() {
        running = true;
        for (int i = 0; i < amountOfThreads; ++i) {
            threads.emplace_back([] {
                while (running) {
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(taskMutex);
                        if (functionQueue.empty()) {
                            cv.wait(lock);
                        } else {
                            task = *functionQueue.begin();
                            functionQueue.pop_front();
                        }
                    }
                    if (task) {
                        task();
                    }
                }
            });
        }
    }

    /**
     *
     * @param func
     */
    void post(function<void()> func) {
        unique_lock<mutex> lock(taskMutex);
        functionQueue.emplace_back(func);
        cv.notify_one();
    }

    /**
     *
     */
    void join() {
        stop();
        for (thread &thread : threads) {
            thread.join();
        }
    }

private:

    void stop() {
        running = false;
        cv.notify_all();
    }


};


#endif //OVING4_WORKERS_H
