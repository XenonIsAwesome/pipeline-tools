#pragma once

#include <utils/queues/IQueue.hpp>

#include "utils/exceptions/bad_any_cast_with_info.hpp"

namespace pt::utils::queues {
    template<typename T>
    class Queue: public IQueue {
    public:
        std::any& peek_any() override {
            T& result = peek();
            return result;
        }

        bool pop_any(std::any& item) override {
            return pop(item);
        }

        bool push_any(const std::any &item) override {
            auto cast_item = utils::any_cast_with_info<T>(item);
            return push(cast_item);
        }

        bool push_any(const std::any &&item) override {
            auto cast_item = utils::any_cast_with_info<T>(item);
            return push(cast_item);
        }

        virtual T& peek() = 0;
        virtual bool pop(T&) = 0;
        virtual bool push(const T&) = 0;
        virtual bool push(const T&&) = 0;
    };
}
