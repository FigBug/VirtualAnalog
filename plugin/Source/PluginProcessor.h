#pragma once

#include <JuceHeader.h>

#include "VirtualAnalogVoice.h"

//==============================================================================
class VirtualAnalogAudioProcessor : public gin::GinProcessor,
                                    public MPESynthesiser
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

    //==============================================================================
    void handleMidiEvent (const MidiMessage& m) override;
    void handleController (int ch, int num, int val) override;
    //==============================================================================

    void applyEffects (AudioSampleBuffer& buffer);

    // Voice Params
    struct OSCParams
    {
        OSCParams() = default;

        gin::Parameter::Ptr enable , wave, voices, tune, finetune, level,
                            pulsewidth, detune, spread, pan;

        void setup (VirtualAnalogAudioProcessor& p, int idx);

        JUCE_DECLARE_NON_COPYABLE (OSCParams)
    };

    struct FilterParams
    {
        FilterParams() = default;

        gin::Parameter::Ptr enable, type, keyTracking, velocityTracking,
                            frequency, resonance, amount,
                            attack, decay, sustain, release;

        void setup (VirtualAnalogAudioProcessor& p, int idx);

        JUCE_DECLARE_NON_COPYABLE (FilterParams)
    };

    struct EnvParams
    {
        EnvParams() = default;

        gin::Parameter::Ptr enable, attack, decay, sustain, release;

        void setup (VirtualAnalogAudioProcessor& p, int idx);

        JUCE_DECLARE_NON_COPYABLE (EnvParams)
    };
    
    struct LFOParams
    {
        LFOParams() = default;

        gin::Parameter::Ptr enable, sync, wave, rate, beat, depth, phase, offset, fade, delay;

        void setup (VirtualAnalogAudioProcessor& p, int idx);

        JUCE_DECLARE_NON_COPYABLE (LFOParams)
    };

    struct ADSRParams
    {
        ADSRParams() = default;

        gin::Parameter::Ptr attack, decay, sustain, release, velocityTracking;

        void setup (VirtualAnalogAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (ADSRParams)
    };

    // Global Params
    struct GlobalParams
    {
        GlobalParams() = default;

        gin::Parameter::Ptr mode, legato, level, voices;

        void setup (VirtualAnalogAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (GlobalParams)
    };

    struct ChorusParams
    {
        ChorusParams() = default;

        gin::Parameter::Ptr enable, delay, rate, depth, width, mix;

        void setup (VirtualAnalogAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (ChorusParams)
    };

    struct DistortionParams
    {
        DistortionParams() = default;

        gin::Parameter::Ptr enable, amount, highpass, output, mix;

        void setup (VirtualAnalogAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (DistortionParams)
    };

    struct EQParams
    {
        EQParams() = default;

        gin::Parameter::Ptr enable,
                            loFreq, loGain, loQ,
                            mid1Freq, mid1Gain, mid1Q,
                            mid2Freq, mid2Gain, mid2Q,
                            hiFreq, hiGain, hiQ;

        void setup (VirtualAnalogAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (EQParams)
    };

    struct CompressorParams
    {
        CompressorParams() = default;

        gin::Parameter::Ptr enable, attack, release, ratio, threshold, gain;

        void setup (VirtualAnalogAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (CompressorParams)
    };

    struct DelayParams
    {
        DelayParams() = default;

        gin::Parameter::Ptr enable, sync, time, beat, fb, cf, mix, delay;

        void setup (VirtualAnalogAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (DelayParams)
    };

    struct ReverbParams
    {
        ReverbParams() = default;

        gin::Parameter::Ptr enable, damping, freezeMode, roomSize, width, mix;

        void setup (VirtualAnalogAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (ReverbParams)
    };

    struct LimiterParams
    {
        LimiterParams() = default;

        gin::Parameter::Ptr enable, attack, release, threshold, gain;

        void setup (VirtualAnalogAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (LimiterParams)
    };

    //==============================================================================
    int modSrcPressure = 0, modSrcTimbre = 0, modScrPitchBend = 0,
        modSrcNote = 0, modSrcVelocity = 0;

    int modSrcCC[119]                    = {0};
    int modSrcMonoLFO[Cfg::numLFOs]      = {0};
    int modSrcLFO[Cfg::numLFOs]          = {0};
    int modSrcFilter[Cfg::numFilters]    = {0};
    int modSrcEnv[Cfg::numENVs]          = {0};

    //==============================================================================

    OSCParams oscParams[Cfg::numOSCs];
    FilterParams filterParams[Cfg::numFilters];
    EnvParams envParams[Cfg::numENVs];
    LFOParams lfoParams[Cfg::numLFOs];

    ADSRParams adsrParams;

    GlobalParams globalParams;
    ChorusParams chorusParams;
    DistortionParams distortionParams;
    EQParams eqParams;
    CompressorParams compressorParams;
    DelayParams delayParams;
    ReverbParams reverbParams;
    LimiterParams limiterParams;

    //==============================================================================
    gin::Modulation chorus { 0.5f };
    gin::Distortion distortion;
    gin::StereoDelay stereoDelay { 120.1 };
    gin::Dynamics compressor;
    gin::Dynamics limiter;
    gin::EQ eq {4};
    Reverb reverb;
    gin::GainProcessor outputGain;

    //==============================================================================
    gin::ModMatrix modMatrix;

    gin::LFO modLFOs[Cfg::numLFOs];

    AudioPlayHead* playhead = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualAnalogAudioProcessor)
};

