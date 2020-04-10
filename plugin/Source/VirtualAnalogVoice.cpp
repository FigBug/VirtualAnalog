#include "VirtualAnalogVoice.h"
#include "PluginProcessor.h"

using namespace gin;

//==============================================================================
VirtualAnalogVoice::VirtualAnalogVoice (VirtualAnalogAudioProcessor& p, BandLimitedLookupTables& bllt)
    : proc (p)
    , bandLimitedLookupTables (bllt)
{
    for (auto& f : filters)
        f.setNumChannels (2);
}

void VirtualAnalogVoice::noteStarted()
{
    for (auto& osc : oscillators)
        osc.noteOn();

    for (auto& f : filters)
        f.reset();

    for (auto& a : filterADSRs)
    {
        a.reset();
        a.noteOn();
    }

    for (auto& a : modADSRs)
     {
         a.reset();
         a.noteOn();
     }

    adsr.reset();
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

    for (auto& osc : oscillators)
        osc.setSampleRate (newRate);

    for (auto& f : filters)
        f.setSampleRate (newRate);

    for (auto& a : filterADSRs)
        a.setSampleRate (newRate);

    adsr.setSampleRate (newRate);
}

void VirtualAnalogVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    updateParams (numSamples);

    // Run OSC
    ScratchBuffer buffer (2, numSamples);

    for (int i = 0; i < 3; i++)
        oscillators[i].processAdding (currentMidiNotes[i], oscParams[i], buffer);

    // Apply velocity
    buffer.applyGain (velocityToGain (currentlyPlayingNote.noteOnVelocity.asUnsignedFloat()));

    // Apply filters
    for (auto& f : filters)
        f.process (buffer);
    
    // Run ADSR
    adsr.processMultiplying (buffer);
    
    if (adsr.getState() == AnalogADSR::State::idle)
        clearCurrentNote();
    
    // Copy output to synth
    outputBuffer.copyFrom (0, startSample, buffer, 0, 0, numSamples);
    outputBuffer.copyFrom (1, startSample, buffer, 1, 0, numSamples);
}

void VirtualAnalogVoice::updateParams (int blockSize)
{
    auto note = getCurrentlyPlayingNote();

    for (int i = 0; i < 3; i++)
    {
        currentMidiNotes[i] = float (note.initialNote + note.totalPitchbendInSemitones / 100.0);
        currentMidiNotes[i] += getValue (proc.oscParams[i].tune) + getValue (proc.oscParams[i].finetune) / 100.0f;

        oscParams[i].wave   = (Wave) int (proc.oscParams[i].wave->getProcValue());
        oscParams[i].voices = int (proc.oscParams[i].voices->getProcValue());
        oscParams[i].pw     = getValue (proc.oscParams[i].pulsewidth) / 100.0f;
        oscParams[i].pan    = getValue (proc.oscParams[i].pan);
        oscParams[i].spread = getValue (proc.oscParams[i].spread) / 100.0f;
        oscParams[i].detune = getValue (proc.oscParams[i].detune);
        oscParams[i].gain   = Decibels::decibelsToGain (getValue (proc.oscParams[i].level));
    }

    for (int i = 0; i < 2; i++)
    {
        filterADSRs[i].setAttack (getValue (proc.filterParams[i].attack));
        filterADSRs[i].setSustainLevel (getValue (proc.filterParams[i].attack));
        filterADSRs[i].setDecay (getValue (proc.filterParams[i].attack));
        filterADSRs[i].setRelease (getValue (proc.filterParams[i].attack));

        float filterWidth = float (getMidiNoteFromHertz (20000.0));
        float filterEnv   = filterADSRs[i].getOutput();

        float n = getValue (proc.filterParams[i].frequency);
        n += (currentlyPlayingNote.initialNote - 60) * getValue (proc.filterParams[i].keyTracking) / 100.0f;
        n += filterEnv * (getValue (proc.filterParams[i].amount) * filterWidth);

        float f = getMidiNoteInHertz (n);
        float maxFreq = std::min (20000.0f, float (getSampleRate() / 2));
        f = jlimit (4.0f, maxFreq, f);

        float q = Q / (1.0f - (getValue (proc.filterParams[i].resonance) / 100.0f) * 0.99f);

        filters[i].setType((Filter::Type)int (proc.filterParams[i].type->getProcValue()));
        filters[i].setSlope ((Filter::Slope)int (proc.filterParams[i].slope->getProcValue()));
        filters[i].setParams (f, q);

        filterADSRs[i].process (blockSize);
    }

    for (int i = 0; i < 2; i++)
    {
        modADSRs[i].setAttack (getValue (proc.filterParams[i].attack));
        modADSRs[i].setSustainLevel (getValue (proc.filterParams[i].attack));
        modADSRs[i].setDecay (getValue (proc.filterParams[i].attack));
        modADSRs[i].setRelease (getValue (proc.filterParams[i].attack));

        modADSRs[i].process (blockSize);
    }

    adsr.setAttack (getValue (proc.attack));
    adsr.setDecay (getValue (proc.decay));
    adsr.setSustainLevel (getValue (proc.sustain));
    adsr.setRelease (getValue (proc.release));
}
