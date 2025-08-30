#pragma once

#include <any>

namespace pt::utils::queues {
    class IQueue {
    public:
        virtual ~IQueue() = default;

        virtual std::any &peek_any() = 0;

        virtual bool pop_any(std::any &) = 0;

        virtual bool push_any(const std::any &) = 0;

        virtual bool push_any(const std::any &&) = 0;
    };
}
