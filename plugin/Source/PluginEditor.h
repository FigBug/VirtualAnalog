#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualAnalogAudioProcessorEditor)
};
