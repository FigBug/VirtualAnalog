#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Boxes.h"

//==============================================================================
class VirtualAnalogAudioProcessorEditor : public gin::GinAudioProcessorEditor
{
public:
    VirtualAnalogAudioProcessorEditor (VirtualAnalogAudioProcessor&);
    ~VirtualAnalogAudioProcessorEditor() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    VirtualAnalogAudioProcessor& proc;

    gin::ControlHeader commonHeader { "Common" };
    gin::ControlHeader unisonHeader { "Unison" };

    CommonBox common {proc};
    UnisonBox unison {proc};

    gin::ControlHeader oscHeaders[4] =
    {
        { "OSC1" },
        { "OSC2" },
        { "OSC3" },
        { "OSC4" },
    };
    OscillatorBox oscillators[4] =
    {
        { proc, 0 },
        { proc, 1 },
        { proc, 2 },
        { proc, 3 },
    };

    gin::ControlHeader filterHeaders[2] =
    {
        { "Filter 1" },
        { "Filter 2" },
    };
    FilterBox filters[2] =
    {
        { proc, 0 },
        { proc, 1 },
    };

    gin::ControlHeader adsrHeader { "ADSR" };
    ADSRBox adsr { proc };

    gin::ControlHeader modulationHeader { "Modulation" };
    ModulationBox modulation { proc };

    gin::ControlHeader effectsHeader { "Effects" };
    EffectsBox effects { proc };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualAnalogAudioProcessorEditor)
};
