#pragma once

#include <flow/blocks/Aggregator.hpp>
#include <flow/blocks/Module.hpp>
#include <flow/blocks/Source.hpp>
#include <flow/blocks/Sink.hpp>

class MockSink : public pt::flow::Sink<int> {
public:
    MockSink(size_t collect_size = 128): Sink() {
        collected.reserve(collect_size);
    }

    void process(int input) override {
        if (collected.size() < collected.capacity()) {
            collected.emplace_back(input);
        }
    }
    std::vector<int> collected;
};

class CollectSink : public pt::flow::Sink<std::vector<int> > {
public:
    void process(std::vector<int> input) override { collected = input; }
    std::vector<int> collected;
};

class NullSource : public pt::flow::Source<int> {
public:
    std::optional<int> process() override { return std::nullopt; }
};

class NullModule : public pt::flow::Module<int, int> {
public:
    std::optional<int> process(int) override { return std::nullopt; }
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

    Tracer &operator=(Tracer &&o) noexcept {
        value = o.value;
        events.push_back("move=");
        return *this;
    }

    static void reset() { events.clear(); }
};
