#pragma once

#include <JuceHeader.h>

class VirtualAnalogAudioProcessor;

//==============================================================================
class VirtualAnalogVoice : public MPESynthesiserVoice,
                           public gin::ModVoice
{
public:
    VirtualAnalogVoice (VirtualAnalogAudioProcessor& p, gin::BandLimitedLookupTables& bandLimitedLookupTables);
    
    void noteStarted() override;
    void noteStopped (bool allowTailOff) override;

    void notePressureChanged() override     {}
    void notePitchbendChanged() override    {}
    void noteTimbreChanged() override       {}
    void noteKeyStateChanged() override     {}
    
    void setCurrentSampleRate (double newRate) override;

    void renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
private:
    void updateParams (int blockSize);

    VirtualAnalogAudioProcessor& proc;
    gin::BandLimitedLookupTables& bandLimitedLookupTables;

    static const int numOSCs = 3;
    
    gin::VoicedStereoOscillator oscillators[numOSCs] = { bandLimitedLookupTables,
                                                         bandLimitedLookupTables,
                                                         bandLimitedLookupTables };

    gin::Filter filters[2];
    gin::ADSR filterADSRs[2];
    gin::ADSR modADSRs[2];

    gin::AnalogADSR adsr;

    float currentMidiNotes[3];
    gin::VoicedStereoOscillator::Params oscParams[3];
};
