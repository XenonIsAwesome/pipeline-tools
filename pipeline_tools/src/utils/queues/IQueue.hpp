#pragma once

#include <any>

namespace pt::queues {
    template<typename T>
    class IQueue {
    public:
        virtual ~IQueue() = default;

        virtual T& peek() = 0;

        virtual bool pop(T&) = 0;

        virtual bool push(const T&) = 0;

        virtual bool push(const T&&) = 0;
    };
}
