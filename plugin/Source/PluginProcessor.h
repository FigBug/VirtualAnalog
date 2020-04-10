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

    struct OSCParams
    {
        gin::Parameter::Ptr wave, voices, tune, finetune, level, pulsewidth,
                            detune, spread, pan;

        void setup (VirtualAnalogAudioProcessor& p, int idx);
    };

    struct FilterParams
    {
        gin::Parameter::Ptr type, slope, keyTracking, velocityTracking,
                            frequency, resonance, amount,
                            attack, decay, sustain, release;

        void setup (VirtualAnalogAudioProcessor& p, int idx);
    };

    struct EnvParams
    {

        gin::Parameter::Ptr attack, decay, sustain, release;

        void setup (VirtualAnalogAudioProcessor& p, int idx);
    };

    OSCParams oscParams[3];
    FilterParams filterParams[2];
    EnvParams envParams[2];

    gin::Parameter::Ptr attack, decay, sustain, release, velocityTracking;

    gin::ModMatrix modMatrix;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualAnalogAudioProcessor)
};

