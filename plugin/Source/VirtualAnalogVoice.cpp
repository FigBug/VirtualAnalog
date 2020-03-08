#include "VirtualAnalogVoice.h"
#include "PluginProcessor.h"

using namespace gin;

//==============================================================================
VirtualAnalogVoice::VirtualAnalogVoice (VirtualAnalogAudioProcessor& p)
    : proc (p)
    , osc  {proc.bandLimitedLookupTables}
{
}

void VirtualAnalogVoice::noteStarted()
{
    osc.noteOn();
    adsr.noteOn();
}

void VirtualAnalogVoice::noteStopped (bool allowTailOff)
{
    adsr.noteOff();
    
    if (! allowTailOff)
        clearCurrentNote();
}

void VirtualAnalogVoice::setCurrentSampleRate (double newRate)
{
    MPESynthesiserVoice::setCurrentSampleRate (newRate);
    
    osc.setSampleRate (newRate);
    adsr.setSampleRate (newRate);
}

void VirtualAnalogVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    auto note = getCurrentlyPlayingNote();
    auto pitch = float (note.initialNote + note.totalPitchbendInSemitones / 100.0);
    
    // Run OSC
    ScratchBuffer buffer (2, numSamples);
    osc.processAdding (pitch, {}, buffer);
    
    // Run ADSR
    adsr.processMultiplying (buffer);
    
    if (adsr.getState() == AnalogADSR::State::idle)
        clearCurrentNote();
    
    // Copy output to synth
    outputBuffer.copyFrom (0, startSample, buffer, 0, 0, numSamples);
    outputBuffer.copyFrom (1, startSample, buffer, 1, 0, numSamples);
}
