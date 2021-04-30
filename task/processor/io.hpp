#pragma once
#ifndef PCH
    #include <task/processor/base.hpp>
#endif

namespace task
{
    namespace processor
    {
        template <typename Input_queue, typename Output_queue>
        class io: public processor_base<Input_queue>
        {
        public:
            using base = processor_base<Input_queue>;
            using input_queue_t = typename base::input_queue_t;
            using task_item_t = typename base::task_item_t;
            using result_item_t = typename Output_queue::value_type;
            using base::thread_;
            using base::active_;
            using base::input_queue_;

            using output_queue_t = Output_queue;

            io() = default;
            io(const io&) = delete;
            io(io&&) = default;
            io& operator = (const io&) = delete;
            io& operator = (io&&) = default;

            output_queue_t* output_queue() const noexcept { return output_queue_; }
            void set_output_queue(output_queue_t* item) noexcept { output_queue_ = item; }

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
                catch(const std::exception& ex)
                {
                    std::cout << ex.what() << '\n';
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
                            auto&& result = std::move(item());
                            if (result)
                            {
                                output_queue_->push(std::move(result));
                            }
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

            output_queue_t* output_queue_ = nullptr;
        };
    }
}
