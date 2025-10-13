#pragma once

#include <providers/FileProvider.h>
#include <flow/Mission.hpp>

/**
 * In this example, we let the user decide our input and output.
 *
 * Because of that, the dev building this mission, only has to build the
 * logic of the mission, and not care about the input or output,
 * which will be supplied by the providers.
 */
template<pt::flow::concepts::PipelineProvider InputPP, pt::flow::concepts::PipelineProvider OutputPP>
class DSPMission: public pt::flow::Mission<InputPP, OutputPP> {
protected:
    void build() override {
        // Specific mission logic here
    }
};

/**
 * In this example, we provide pt::flow::Mission with void, void (default)
 *
 * Therefor, the dev building this mission will have to provide
 * an input pipeline and an output pipeline
 */
class FullMission: public pt::flow::Mission<> {
    void build() override {
        // Entire mission logic here
    }
};


/// File -> DSPMission -> File mission
DSPMission<FileProvider, FileProvider>;

// TODO: switch after implementation of both providers
using SDRProvider = FileProvider;
using RMQProvider = FileProvider;

/// SDR -> DSPMission -> RMQProvider
DSPMission<SDRProvider, RMQProvider>;
