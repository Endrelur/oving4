#include "Workers.h"

Workers::Workers(int n_threads) {
    this->n_threads = n_threads;
    running = false;
    waiting = false;
}

void Workers::post_timeout(function<void()> func, int time) {
    thread th([this, &time, &func] {
        waiting = true;
        this_thread::sleep_for(chrono::seconds(time));
        cout << "thread waited " << time
             << " seconds before posting" << endl;
        post(func);
        waiting = false;
    });
    th.join();
}

void Workers::post(function<void()> task) {
    {
        unique_lock<mutex> lock(tasks_mutex);
        tasks.emplace_back(task);
    }
    cv.notify_one();
}


void Workers::start() {
    running = true;
    create_workers();
}

void Workers::create_workers() {
    for (int i = 0; i < n_threads; i++)
        create_worker();
}

void Workers::create_worker() {
    worker_threads.emplace_back([this] {
        while (running || !tasks.empty()) {
            function<void()> task;
            {
                unique_lock<mutex> lock(tasks_mutex);
                if (!tasks.empty()) {
                    task = *tasks.begin();
                    tasks.pop_front();
                } else {
                    cv.wait(lock);
                }

            }
            if (task)
                task();
        }
    });
}

void Workers::join() {
    stop();
    for (auto &thread : worker_threads)
        thread.join();
    if (!tasks.empty())
        cout << "ERROR! SOME TASKS WERE NOT FINISHED....\n";
}

void Workers::stop() {
    if (waiting) {
        thread th([this] {
            while (waiting) {
                this_thread::sleep_for(chrono::milliseconds(20));
            }
        });
        th.join();
    }
    running = false;
    cv.notify_all();

}