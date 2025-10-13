#pragma once

#include <flow/PipelineProvider.h>

class FileProvider final: public pt::flow::InputPipelineProvider, public pt::flow::OutputPipelineProvider {
public:
    pt::flow::Pipeline build_input() override {
        // Flow logic for input from file here
        return pt::flow::Pipeline();
    }


    pt::flow::Pipeline build_output() override {
        // Flow logic for output to file here
        return pt::flow::Pipeline();
    }
};
