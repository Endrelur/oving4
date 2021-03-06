#ifndef OVING4_WORKERS_H
#define OVING4_WORKERS_H

#include <iostream>

#include <list>
#include <vector>
#include <functional>
#include <condition_variable>

#include <thread>
#include <mutex>
#include <atomic>

using namespace std;

class Workers {

public:
    Workers(int n_threads);

    void post(function<void()> task);

    void start();

    void join();

    void post_timeout(function<void()> func, int time);

private:
    atomic<bool> running;
    int n_threads;
    vector<thread> worker_threads;

    list<function<void()>> tasks;
    condition_variable cv;
    mutex tasks_mutex;

    void create_workers();

    void create_worker();

    void stop();
};

#endif //OVING4_WORKERS_H
