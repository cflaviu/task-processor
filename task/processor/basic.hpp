#pragma once
#ifndef PCH
    #include <task/processor/base.hpp>
#endif

namespace task
{
    namespace processor
    {
        template <typename Queue>
        class basic: public processor::base<Queue>
        {
        public:
            using base = processor::base<Queue>;
            using input_queue_t = typename base::input_queue_t;
            using task_item_t = typename base::task_item_t;
            using base::thread_;
            using base::input_queue_;

            basic() = default;
            basic(const basic&) = delete;
            basic(basic&&) = default;
            basic& operator = (const basic&) = delete;
            basic& operator = (basic&&) = default;

            void start() noexcept
            {
                try
                {
                    if (input_queue_ != nullptr)
                    {
                        input_queue_->start();
                        thread_ = std::thread([this]() { execute(); });
                    }
                }
                catch(...)
                {}
            }

        protected:
            void execute() noexcept
            {
                task_item_t item;
                bool is_active;
                do
                {
                    try
                    {
                        is_active = input_queue_->pop(item);
                        if (is_active && item)
                        {
                            item();
                        }
                    }
                    catch(const std::exception& ex)
                    {
                        std::cout << ex.what() << '\n';
                    }
                    catch(...)
                    {}
                }
                while(is_active);
            }
        };
    }
}
