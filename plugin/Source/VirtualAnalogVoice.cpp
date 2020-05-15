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
    fastKill = false;
    startVoice();

    auto note = getCurrentlyPlayingNote();
    if (glideInfo.fromNote != -1 && (glideInfo.glissando || glideInfo.portamento))
    {
        noteSmoother.setTime (glideInfo.rate);
        noteSmoother.setValueUnsmoothed (glideInfo.fromNote / 127.0f);
        noteSmoother.setValue (note.initialNote / 127.0f);
    }
    else
    {
        noteSmoother.setValueUnsmoothed (note.initialNote / 127.0f);
    }

    proc.modMatrix.setPolyValue (*this, proc.modSrcVelocity, note.noteOnVelocity.asUnsignedFloat());
    proc.modMatrix.setPolyValue (*this, proc.modSrcTimbre, note.initialTimbre.asUnsignedFloat());
    proc.modMatrix.setPolyValue (*this, proc.modSrcPressure, note.pressure.asUnsignedFloat());
    
    updateParams (0);
    snapParams();
    updateParams (0);
    
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
    
    for (auto& l : modLFOs)
    {
        l.reset();
        l.noteOn();
    }

    adsr.reset();
    adsr.noteOn();
}

void VirtualAnalogVoice::noteRetriggered()
{
    auto note = getCurrentlyPlayingNote();
    
    if (glideInfo.fromNote != -1 && (glideInfo.glissando || glideInfo.portamento))
    {
        noteSmoother.setTime (glideInfo.rate);
        noteSmoother.setValue (note.initialNote / 127.0f);
    }
    else
    {
        noteSmoother.setValueUnsmoothed (note.initialNote / 127.0f);
    }
    
    proc.modMatrix.setPolyValue (*this, proc.modSrcVelocity, note.noteOnVelocity.asUnsignedFloat());
    proc.modMatrix.setPolyValue (*this, proc.modSrcTimbre, note.initialTimbre.asUnsignedFloat());
    proc.modMatrix.setPolyValue (*this, proc.modSrcPressure, note.pressure.asUnsignedFloat());
    
    updateParams (0);

    for (auto& osc : oscillators)
        osc.noteOn();

    for (auto& a : filterADSRs)
        a.noteOn();

    for (auto& a : modADSRs)
         a.noteOn();
    
    adsr.noteOn();
}

void VirtualAnalogVoice::noteStopped (bool allowTailOff)
{
    adsr.noteOff();

    for (auto& a : filterADSRs)
        a.noteOff();

    for (auto& a : modADSRs)
        a.noteOff();

    if (! allowTailOff)
    {
        clearCurrentNote();
        stopVoice();
    }
}

void VirtualAnalogVoice::notePressureChanged()
{
    auto note = getCurrentlyPlayingNote();
    proc.modMatrix.setPolyValue (*this, proc.modSrcPressure, note.pressure.asUnsignedFloat());
}

void VirtualAnalogVoice::noteTimbreChanged()
{
    auto note = getCurrentlyPlayingNote();
    proc.modMatrix.setPolyValue (*this, proc.modSrcTimbre, note.initialTimbre.asUnsignedFloat());
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
    
    for (auto& l : modLFOs)
        l.setSampleRate (newRate);

    noteSmoother.setSampleRate (newRate);
    adsr.setSampleRate (newRate);
}

void VirtualAnalogVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    updateParams (numSamples);

    // Run OSC
    ScratchBuffer buffer (2, numSamples);

    for (int i = 0; i < Cfg::numOSCs; i++)
        if (proc.oscParams[i].enable->isOn())
            oscillators[i].processAdding (currentMidiNotes[i], oscParams[i], buffer);

    // Apply velocity
    buffer.applyGain (velocityToGain (currentlyPlayingNote.noteOnVelocity.asUnsignedFloat()));

    // Apply filters
    for (int i = 0; i < numElementsInArray (filters); i++)
        if (proc.filterParams[i].enable->isOn())
            filters[i].process (buffer);
    
    // Run ADSR
    adsr.processMultiplying (buffer);
    
    if (adsr.getState() == AnalogADSR::State::idle)
    {
        clearCurrentNote();
        stopVoice();
    }

    // Copy output to synth
    outputBuffer.addFrom (0, startSample, buffer, 0, 0, numSamples);
    outputBuffer.addFrom (1, startSample, buffer, 1, 0, numSamples);
    
    finishBlock (numSamples);
}

void VirtualAnalogVoice::updateParams (int blockSize)
{
    auto note = getCurrentlyPlayingNote();
    
    proc.modMatrix.setPolyValue (*this, proc.modSrcNote, note.initialNote / 127.0f);

    for (int i = 0; i < Cfg::numOSCs; i++)
    {
        if (! proc.oscParams[i].enable->isOn()) continue;
        
        currentMidiNotes[i] = noteSmoother.getCurrentValue() * 127.0f;
        if (glideInfo.glissando) currentMidiNotes[i] = roundToInt (currentMidiNotes[i]);
        currentMidiNotes[i] += float (note.totalPitchbendInSemitones / 100.0);
        currentMidiNotes[i] += getValue (proc.oscParams[i].tune) + getValue (proc.oscParams[i].finetune) / 100.0f;

        oscParams[i].wave   = (Wave) int (proc.oscParams[i].wave->getProcValue());
        oscParams[i].voices = int (proc.oscParams[i].voices->getProcValue());
        oscParams[i].vcTrns = int (proc.oscParams[i].voicesTrns->getProcValue());
        oscParams[i].pw     = getValue (proc.oscParams[i].pulsewidth) / 100.0f;
        oscParams[i].pan    = getValue (proc.oscParams[i].pan);
        oscParams[i].spread = getValue (proc.oscParams[i].spread) / 100.0f;
        oscParams[i].detune = getValue (proc.oscParams[i].detune);
        oscParams[i].gain   = Decibels::decibelsToGain (getValue (proc.oscParams[i].level));
    }

    for (int i = 0; i < Cfg::numFilters; i++)
    {
        if (! proc.filterParams[i].enable->isOn()) continue;
        
        filterADSRs[i].setAttack (getValue (proc.filterParams[i].attack));
        filterADSRs[i].setSustainLevel (getValue (proc.filterParams[i].sustain));
        filterADSRs[i].setDecay (getValue (proc.filterParams[i].decay));
        filterADSRs[i].setRelease (getValue (proc.filterParams[i].release));

        float filterWidth = float (getMidiNoteFromHertz (20000.0));
        float filterEnv   = filterADSRs[i].getOutput();

        float n = getValue (proc.filterParams[i].frequency);
        n += (currentlyPlayingNote.initialNote - 60) * getValue (proc.filterParams[i].keyTracking) / 100.0f;
        n += filterEnv * (getValue (proc.filterParams[i].amount) * filterWidth);

        float f = getMidiNoteInHertz (n);
        float maxFreq = std::min (20000.0f, float (getSampleRate() / 2));
        f = jlimit (4.0f, maxFreq, f);

        float q = Q / (1.0f - (getValue (proc.filterParams[i].resonance) / 100.0f) * 0.99f);

        switch (int (proc.filterParams[i].type->getProcValue()))
        {
            case 0:
                filters[i].setType (Filter::lowpass);
                filters[i].setSlope (Filter::db12);
                break;
            case 1:
                filters[i].setType (Filter::lowpass);
                filters[i].setSlope (Filter::db24);
                break;
            case 2:
                filters[i].setType (Filter::highpass);
                filters[i].setSlope (Filter::db12);
                break;
            case 3:
                filters[i].setType (Filter::highpass);
                filters[i].setSlope (Filter::db24);
                break;
            case 4:
                filters[i].setType (Filter::bandpass);
                filters[i].setSlope (Filter::db12);
                break;
            case 5:
                filters[i].setType (Filter::bandpass);
                filters[i].setSlope (Filter::db24);
                break;
            case 6:
                filters[i].setType (Filter::notch);
                filters[i].setSlope (Filter::db12);
                break;
            case 7:
                filters[i].setType (Filter::notch);
                filters[i].setSlope (Filter::db24);
                break;
        }
        
        filters[i].setParams (f, q);

        proc.modMatrix.setPolyValue (*this, proc.modSrcFilter[i], filterADSRs[i].getOutput());
        
        filterADSRs[i].process (blockSize);
    }

    for (int i = 0; i < Cfg::numENVs; i++)
    {
        if (proc.filterParams[i].enable->isOn())
        {
            modADSRs[i].setAttack (getValue (proc.filterParams[i].attack));
            modADSRs[i].setSustainLevel (getValue (proc.filterParams[i].attack));
            modADSRs[i].setDecay (getValue (proc.filterParams[i].attack));
            modADSRs[i].setRelease (getValue (proc.filterParams[i].attack));

            proc.modMatrix.setPolyValue (*this, proc.modSrcEnv[i], modADSRs[i].getOutput());

            modADSRs[i].process (blockSize);
        }
        else
        {
            proc.modMatrix.setPolyValue (*this, proc.modSrcEnv[i], 0.0f);
        }
    }
    
    for (int i = 0; i < Cfg::numLFOs; i++)
    {
        if (proc.lfoParams[i].enable->isOn())
        {
            LFO::Parameters params;

            float freq = 0;
            if (proc.lfoParams[i].sync->getProcValue() > 0.0f)
                freq = 1.0f / NoteDuration::getNoteDurations()[size_t (proc.lfoParams[i].beat->getProcValue())].toSeconds (proc.playhead);
            else
                freq = getValue (proc.lfoParams[i].rate);

            params.waveShape = (LFO::WaveShape) int (proc.lfoParams[i].wave->getProcValue());
            params.frequency = freq;
            params.phase     = getValue (proc.lfoParams[i].phase);
            params.offset    = getValue (proc.lfoParams[i].offset);
            params.depth     = getValue (proc.lfoParams[i].depth);
            params.delay     = getValue (proc.lfoParams[i].delay);
            params.fade      = getValue (proc.lfoParams[i].fade);

            modLFOs[i].setParameters (params);
            modLFOs[i].process (blockSize);

            proc.modMatrix.setPolyValue (*this, proc.modSrcLFO[i], modLFOs[i].getOutput());
        }
        else
        {
            proc.modMatrix.setPolyValue (*this, proc.modSrcLFO[i], 0);
        }
    }

    adsr.setAttack (getValue (proc.adsrParams.attack));
    adsr.setDecay (getValue (proc.adsrParams.decay));
    adsr.setSustainLevel (getValue (proc.adsrParams.sustain));
    adsr.setRelease (fastKill ? 0.01f : getValue (proc.adsrParams.release));
    
    noteSmoother.process (blockSize);
}

bool VirtualAnalogVoice::isVoiceActive()
{
    return isActive();
}

float VirtualAnalogVoice::getFilterCutoffNormalized (int idx)
{
    float freq = filters[idx].getFrequency();
    return proc.filterParams[idx].frequency->getUserRange().convertTo0to1 (getMidiNoteFromHertz (freq));
}
