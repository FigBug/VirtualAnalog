#pragma once

#include <JuceHeader.h>

//==============================================================================
class VirtualAnalogAudioProcessor : public gin::GinProcessor                                    
{
public:
    //==============================================================================
    VirtualAnalogAudioProcessor();
    ~VirtualAnalogAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    gin::BandLimitedLookupTables bandLimitedLookupTables;
    MPESynthesiser synth;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualAnalogAudioProcessor)
};

