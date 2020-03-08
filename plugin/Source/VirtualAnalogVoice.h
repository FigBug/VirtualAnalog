#pragma once

#include <JuceHeader.h>

class VirtualAnalogAudioProcessor;

//==============================================================================
class VirtualAnalogVoice : public MPESynthesiserVoice
{
public:
    VirtualAnalogVoice (VirtualAnalogAudioProcessor& p);
    
    void noteStarted() override;
    void noteStopped (bool allowTailOff) override;

    void notePressureChanged() override     {}
    void notePitchbendChanged() override    {}
    void noteTimbreChanged() override       {}
    void noteKeyStateChanged() override     {}
    
    void setCurrentSampleRate (double newRate) override;

    void renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
private:
    VirtualAnalogAudioProcessor& proc;
    
    gin::VoicedStereoOscillator osc;
    gin::AnalogADSR adsr;
};
