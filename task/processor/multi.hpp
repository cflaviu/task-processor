#pragma once
#ifndef PCH
    #include <algorithm>
    #include <vector>
#endif

namespace task
{
    namespace processor
    {
        template <typename Processor>
        class multi
        {
            multi(const multi&) = delete;
            multi& operator = (const multi&) = delete;

        public:
            using processor_t = Processor;
            using processor_array = std::vector<processor_t>;

            multi(size_t processor_count = std::thread::hardware_concurrency()):
                processors_(processor_count)
            {}

            multi(multi&&) = default;
            multi& operator = (multi&&) = default;

            const processor_array& processors() const noexcept
            {
                return processors_;
            }

            processor_array& processors() noexcept
            {
                return processors_;
            }

            void start()
            {
                std::for_each(processors_.begin(), processors_.end(), [](auto& item) noexcept
                {
                    item.start();
                });
            }

            void join()
            {
                std::for_each(processors_.begin(), processors_.end(), [](auto& item)
                {
                    item.join();
                });
            }

            void stop()
            {
                std::for_each(processors_.begin(), processors_.end(), [](auto& item) noexcept
                {
                    item.stop();
                });
            }

        protected:
            processor_array processors_;
        };
    }
}
