#pragma once

#include <flow/blocks/Aggregator.hpp>
#include <flow/blocks/Module.hpp>
#include <flow/blocks/Source.hpp>
#include <flow/blocks/Sink.hpp>

class MockSink : public pt::flow::Sink<int> {
public:
    void process(int input) override { collected.push_back(input); }
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
