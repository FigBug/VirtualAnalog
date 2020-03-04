#pragma once

#include <JuceHeader.h>

//==============================================================================
class VirtualAnalogVoice : public MPESynthesiserVoice
{
public:
    VirtualAnalogVoice();
    
    void noteStarted() override;
    void noteStopped (bool allowTailOff) override;

    void notePressureChanged() override     {}
    void notePitchbendChanged() override    {}
    void noteTimbreChanged() override       {}
    void noteKeyStateChanged() override     {}
    
    void setCurrentSampleRate (double newRate) override;

    void renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
private:
    gin::AnalogADSR adsr;
};

//==============================================================================
class VirtualAnalogAudioProcessor : public gin::GinProcessor,
                                    private MPESynthesiser
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

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualAnalogAudioProcessor)
};
