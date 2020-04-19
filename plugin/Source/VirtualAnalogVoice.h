#pragma once

#include <JuceHeader.h>

class VirtualAnalogAudioProcessor;

//==============================================================================
class VirtualAnalogVoice : public MPESynthesiserVoice,
                           public gin::ModVoice
{
public:
    static const int numOSCs    = 4;
    static const int numFilters = 2;
    static const int numENVs    = 2;
    static const int numLFOs    = 2;

    VirtualAnalogVoice (VirtualAnalogAudioProcessor& p, gin::BandLimitedLookupTables& bandLimitedLookupTables);
    
    void noteStarted() override;
    void noteStopped (bool allowTailOff) override;

    void notePressureChanged() override;
    void noteTimbreChanged() override;
    void notePitchbendChanged() override    {}
    void noteKeyStateChanged() override     {}
    
    void setCurrentSampleRate (double newRate) override;

    void renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
private:
    void updateParams (int blockSize);

    VirtualAnalogAudioProcessor& proc;
    gin::BandLimitedLookupTables& bandLimitedLookupTables;

    gin::VoicedStereoOscillator oscillators[numOSCs] =
    {
        bandLimitedLookupTables,
        bandLimitedLookupTables,
        bandLimitedLookupTables,
        bandLimitedLookupTables,
    };

    gin::Filter filters[numFilters];
    gin::ADSR filterADSRs[numFilters];
    
    gin::ADSR modADSRs[numENVs];
    gin::LFO modLFOs[numLFOs];

    gin::AnalogADSR adsr;

    float currentMidiNotes[numOSCs];
    gin::VoicedStereoOscillator::Params oscParams[numOSCs];
};
