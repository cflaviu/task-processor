#pragma once
#ifndef PCH
    #include <thread>
#endif

namespace task
{
    namespace processor
    {
        template <typename Input_queue>
        class base
        {
            base(const base&) = delete;
            base& operator = (const base&) = delete;

        public:
            using input_queue_t = Input_queue;
            using task_item_t = typename Input_queue::value_type;

            base() = default;
            base(base&&) = default;
            base& operator = (base&&) = default;

            ~base() noexcept
            {
                stop();
            }

            bool is_active() const noexcept { return input_queue_ != nullptr && input_queue_->is_active(); }

            input_queue_t* input_queue() const noexcept { return input_queue_; }
            void set_input_queue(input_queue_t* item) noexcept { input_queue_ = item; }

            void stop() noexcept
            {
                if (is_active())
                {
                    input_queue_->stop();
                }

                thread_.join();
            }

            void join()
            {
                thread_.join();
            }

        protected:
            std::thread thread_{};
            input_queue_t* input_queue_ = nullptr;
        };
    }
}
