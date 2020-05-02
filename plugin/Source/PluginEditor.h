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

    CommonBox common { *this, proc };

    gin::ControlHeader oscHeaders = { "Oscillators" };
    OscillatorBox oscillators     = { *this, proc };

    gin::ControlHeader filterHeaders { "Filters" };
    
    FilterBox filters { *this, proc };
    gin::ControlHeader adsrHeader { "ADSR" };
    ADSRBox adsr { *this, proc };

    gin::ControlHeader modulationHeader { "Modulation" };
    ModulationBox modulation { *this, proc };

    gin::ControlHeader effectsHeader { "Effects" };
    EffectsBox effects { *this, proc };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualAnalogAudioProcessorEditor)
};
