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
#include <list>
#include <condition_variable>


using namespace std;

 list <function<void()>> functionQueue;
 vector<thread> threads;
 mutex taskMutex;
 condition_variable cv;
 atomic<bool> running;
 int amountOfThreads;

class Workers {

public:

    Workers(int threadAmount) {
        amountOfThreads = threadAmount;
        running = false;
    }


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

    void post(function<void()> func) {
        unique_lock<mutex> lock(taskMutex);
        functionQueue.emplace_back(func);
        cv.notify_one();
    }

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
