#pragma once

#include <JuceHeader.h>

#include "VirtualAnalogVoice.h"

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

    void updateParams (int blockSize);
    void setupModMatrix();

    gin::BandLimitedLookupTables bandLimitedLookupTables;
    MPESynthesiser synth;

    //==============================================================================
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
    
    struct LFOParams
    {
        gin::Parameter::Ptr sync, wave, rate, beat, depth, phase, offset, fade, delay;

        void setup (VirtualAnalogAudioProcessor& p, int idx);
    };

    struct ADSRParams
    {
        gin::Parameter::Ptr attack, decay, sustain, release, velocityTracking;

        void setup (VirtualAnalogAudioProcessor& p);
    };

    //==============================================================================
    int modSrcPressure = 0, modSrcTimbre = 0, modSrcModWheel = 0, modScrPitchBend = 0,
        modSrcNote = 0, modSrcVelocity = 0;

    int modSrcCC[119]                                   = {0};
    int modSrcMonoLFO[VirtualAnalogVoice::numLFOs]      = {0};
    int modSrcLFO[VirtualAnalogVoice::numLFOs]          = {0};
    int modSrcFilter[VirtualAnalogVoice::numFilters]    = {0};
    int modSrcEvn[VirtualAnalogVoice::numADSRs]         = {0};

    //==============================================================================

    OSCParams oscParams[VirtualAnalogVoice::numOSCs];
    FilterParams filterParams[VirtualAnalogVoice::numFilters];
    EnvParams envParams[VirtualAnalogVoice::numADSRs];
    LFOParams lfoParams[VirtualAnalogVoice::numLFOs];

    ADSRParams adsrParams;

    gin::ModMatrix modMatrix;

    gin::LFO modLFOs[VirtualAnalogVoice::numLFOs];

    AudioPlayHead* playhead = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualAnalogAudioProcessor)
};

