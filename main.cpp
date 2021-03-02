#include <iostream>
#include <list>
#include <condition_variable>
#include <vector>
#include "Workers.h"
#include <thread>


using namespace std;


list<function<void()>> functionQueue;
vector<thread> threads;
mutex taskMutex;
condition_variable cv;
atomic<bool> running;
int amountOfThreads;


void start() {
    running = true;
    for (int i = 0; i < amountOfThreads; ++i) {
        threads.emplace_back([] {
            while (running) {
                function<void()> task;
                {
                    unique_lock<mutex> lock(taskMutex);
                    if (!functionQueue.empty()) {
                        task = *functionQueue.begin();
                        functionQueue.pop_front();
                    } else {
                        cv.wait(lock);
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

void stop() {
    running = false;
    cv.notify_all();
}

void join() {
    stop();
    for (thread &thread : threads) {
        thread.join();
    }
}


int main() {
    amountOfThreads = 4;
    start();
    for (static int i = 0; i < 5; i++) {
        post([] {
            cout << "task " << i
                 << " runs in thread "
                 << this_thread::get_id()
                 << endl;
        });

    }
    join();
}

