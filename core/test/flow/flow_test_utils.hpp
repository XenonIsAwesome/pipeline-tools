#pragma once

#include <flow/blocks/Aggregator.hpp>
#include <flow/blocks/Sink.hpp>
#include <flow/blocks/Source.hpp>
#include <iostream>

template<typename T>
class MockSink : public pt::flow::Sink<T> {
public:
    MockSink(size_t vec_size = 128) : pt::flow::Sink<T>(), collected() {
        collected.reserve(vec_size);
    }

    void process(T input) override {
        if (collected.size() < collected.capacity()) {
            collected.emplace_back(std::move(input));
        }
    }
    std::vector<T> collected;
};


class Tracer {
public:
    static inline std::vector<std::string> events;

    int value;

    Tracer(int v = 0): value(v) {
        events.push_back("ctor");
    }

    Tracer(const Tracer &o): value(o.value) {
        events.push_back("copy");
    }

    Tracer(Tracer &&o) noexcept: value(o.value) {
        events.push_back("move");
    }

    ~Tracer() {
        events.push_back("dtor");
    }

    Tracer &operator=(const Tracer &o) {
        value = o.value;
        events.push_back("copy=");
        return *this;
    }

    Tracer &operator=(Tracer &&o) {
        value = o.value;
        events.push_back("move=");
        return *this;
    }

    static void reset() { events.clear(); }
};

class TracerSource: public pt::flow::Source<Tracer> {
public:
    std::optional<Tracer> process() override {
        return Tracer{42};
    }
};

class TracerSink: public pt::flow::Sink<Tracer> {
public:
    void process(Tracer t) override {
        collected.push_back(t.value);
    }

    std::vector<int> collected;
};