#include <iostream>
#include "Workers.h"

using namespace std;

int main() {
    Workers worker_threads(4);
    Workers event_loop(1);
    worker_threads.start(); // Create 4 internal threads
    event_loop.start(); // Create 1 internal thread
    worker_threads.post([] { //Task A
        cout << "task A"
             << " runs in worker_thread "
             << this_thread::get_id()
             << endl;

    });

    worker_threads.post([] { // Task B
        cout << "task B"
             << " runs in worker_thread "
             << this_thread::get_id()
             << endl;

    });

    event_loop.post([] { // Task C
        cout << "task C"
             << " runs in event_loop_thread "
             << this_thread::get_id()
             << endl;

    });

    event_loop.post([] { // Task D
        cout << "task D"
             << " runs in event_loop_thread "
             << this_thread::get_id()
             << endl;
    });

    worker_threads.post_timeout([] {
        cout << "task E"
             << " runs in worker_thread "
             << this_thread::get_id()
             << endl;
    }, 10);

    event_loop.post_timeout([] {
        cout << "task F"
             << " runs in event_loop_thread "
             << this_thread::get_id()
             << endl;
    }, 2);

    worker_threads.join(); // Calls join() on the worker threads
    event_loop.join(); // Calls join() on the event thread
}



