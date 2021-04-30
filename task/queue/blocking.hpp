#pragma once
#ifndef PCH
    #include <atomic>
    #include <condition_variable>
    #include <deque>
    #include <mutex>
#endif

namespace task
{
    namespace queue
    {
        template <typename T>
        class blocking
        {
            blocking(const blocking &) = delete;
            blocking& operator = (const blocking&) = delete;

         public:
            using value_type = T;
            using lock_t = std::unique_lock<std::mutex>;

            blocking(size_t capacity): capacity_(capacity) {}
            blocking(blocking&&) = default;
            blocking& operator = (blocking &&) = default;

            bool is_active() const noexcept { return active_; }

            void start()
            {
                active_ = true;
            }

            void stop()
            {
                if (active_)
                {
                    active_ = false;
                    not_empty_.notify_all();
                    not_full_.notify_all();
                }
            }

            bool push(T &&item)
            {
                {
                    lock_t lock(deque_mutex_);
                    not_full_.wait(lock, [this]() { return deque_.size() < capacity_ || !active_; });
                    if (active_)
                    {
                        deque_.push_back(std::move(item));
                    }
                }

                if (active_)
                {
                    not_empty_.notify_one();
                }

                return active_;
            }

            bool try_push(T &&item)
            {
                {
                    lock_t lock(deque_mutex_);
                    if (deque_.size() == capacity_ || !active_)
                    {
                        return false;
                    }

                    deque_.push_back(std::move(item));
                }

                if (active_)
                {
                    not_empty_.notify_one();
                }

                return true;
            }

            bool pop(T &item)
            {
                {
                    lock_t lock(deque_mutex_);
                    not_empty_.wait(lock, [this]() { return !deque_.empty() || !active_; });
                    if (active_)
                    {
                        item = std::move(deque_.front());
                        deque_.pop_front();
                    }
                }

                if (active_)
                {
                    not_full_.notify_one();
                }

                return active_;
            }

            bool try_pop(T &item)
            {
                {
                    lock_t lock(deque_mutex_);
                    if (deque_.empty() || !active_)
                    {
                        return false;
                    }

                    item = std::move(deque_.front());
                    deque_.pop_front();
                }

                not_full_.notify_one();
                return true;
            }

        protected:
            std::condition_variable not_empty_;
            std::condition_variable not_full_;
            std::mutex deque_mutex_;
            std::deque<value_type> deque_;
            size_t capacity_;
            std::atomic_bool active_ = true;
        };
    }
}
