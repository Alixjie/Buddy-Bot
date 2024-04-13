#include <functional>
#include <iostream>
#include <thread>

#include "../sync_queue.h"
void producter(SyncQueue<int>& q)
{
    for (int i = 1; i < 11; ++i) {
        // std::cout << q.size() << '\n';
        q.push(i);
        std::cout << "Producter push value: " << i << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
}

void consumer(SyncQueue<int>& q)
{
    int value;
    while (true) {
        if (q.tryPop(value)) {
            std::cout << "Consumer get value: " << value << '\n';
        }
        if (value == 10) {
            break;
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main(int argc, char* argv[])
{
    SyncQueue<int> queue(3);
    std::cout << "Start." << '\n';
    // std::cout<<queue.size()<<std::endl;

    std::thread t1(producter, std::ref(queue));
    std::thread t2(consumer, std::ref(queue));
    t1.join();
    t2.join();
    std::cout << "Stop." << '\n';

    return 0;
}