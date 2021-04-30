#include <iostream>
#include <chrono>
#include <task/queue/blocking.hpp>
#include <task/processor/basic.hpp>
#include <task/processor/multi.hpp>
#include <signal.h>

using namespace std::chrono_literals;
std::mutex cout_mutex;

struct task_item
{
    uint64_t v;

    task_item()
    {
        v = reinterpret_cast<uint64_t>(this);
    }

    operator bool () const noexcept
    {
        return true;
    }

    void operator () ()
    {
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "executing " << std::hex << v << '\n';
        }

        std::this_thread::sleep_for(100ms);
    }
};

using queue_t = task::queue::blocking<task_item>;
using processor_t = task::processor::basic<queue_t>;
using multi_processor_t = task::processor::multi<processor_t>;

processor_t p;

void my_handler(sig_atomic_t)
{
    p.stop();
}

int main()
{
    signal (SIGINT, my_handler);

    queue_t q(10);

#if 1
    multi_processor_t m(1);
    for(auto& i : m.processors())
    {
        i.set_input_queue(&q);
    }

    m.start();
    m.join();
#else
    q.push({});
    q.push({});
    q.push({});

    p.set_input_queue(&q);
    p.start();
    p.join();
#endif

    return 0;
}
