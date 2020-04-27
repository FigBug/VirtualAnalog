#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class CommonBox : public gin::ControlBox
{
public:
    CommonBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc)
        : gin::ControlBox (e)
    {
        add (level = new gin::Knob (proc.globalParams.level, true));
    }

    void resized() override
    {
        level->setBounds (getGridArea (0, 0));
    }

    ParamComponentPtr level;
};

//==============================================================================
class UnisonBox : public gin::ControlBox
{
public:
    UnisonBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc)
        : gin::ControlBox (e)
    {
        for (int i = 0; i < 4; i++)
        {
            auto& osc = proc.oscParams[i];

            add (voices[i] = new gin::Knob (osc.voices));
            add (detune[i] = new gin::Knob (osc.detune));
            add (spread[i] = new gin::Knob (osc.spread));
        }
        setOSC (0);
    }

    void resized() override
    {
        for (int i = 0; i < 4; i++)
        {
            voices[i]->setBounds (getGridArea (1, 0));
            detune[i]->setBounds (getGridArea (0, 1));
            spread[i]->setBounds (getGridArea (1, 1));
        }
    }

    void setOSC (int osc)
    {
        for (int i = 0; i < 4; i++)
        {
            voices[i]->setVisible (osc == i);
            detune[i]->setVisible (osc == i);
            spread[i]->setVisible (osc == i);
        }
    }

    ParamComponentPtr voices[4], detune[4], spread[4];
};

//==============================================================================
class OscillatorBox : public gin::ControlBox
{
public:
    OscillatorBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc, int idx)
        : gin::ControlBox (e)
    {
        auto& osc = proc.oscParams[idx];

        add (wave       = new gin::Select (osc.wave));
        add (tune       = new gin::Knob (osc.tune, true));
        add (finetune   = new gin::Knob (osc.finetune, true));
        add (pan        = new gin::Knob (osc.pan, true));
        add (level      = new gin::Knob (osc.level));
        add (pulsewidth = new gin::Knob (osc.pulsewidth));
    }

    void resized() override
    {
        wave->setBounds (getGridArea (0, 0));
        tune->setBounds (getGridArea (1, 0));
        finetune->setBounds (getGridArea (2, 0));
        pan->setBounds (getGridArea (0, 1));
        level->setBounds (getGridArea (1, 1));
        pulsewidth->setBounds (getGridArea (2, 1));
    }

    ParamComponentPtr wave, tune, finetune, level, pulsewidth, pan;
};

//==============================================================================
class FilterBox : public gin::ControlBox
{
public:
    FilterBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc, int idx)
        : gin::ControlBox (e)
    {
        auto& flt = proc.filterParams[idx];

        add (type             = new gin::Select (flt.type));
        add (frequency        = new gin::Knob (flt.frequency));
        add (resonance        = new gin::Knob (flt.resonance));

        add (keyTracking      = new gin::Knob (flt.keyTracking));
        add (velocityTracking = new gin::Knob (flt.velocityTracking));
        add (amount           = new gin::Knob (flt.amount));

        add (attack           = new gin::Knob (flt.attack));
        add (decay            = new gin::Knob (flt.decay));
        add (sustain          = new gin::Knob (flt.sustain));
        add (release          = new gin::Knob (flt.release));
    }

    void resized() override
    {
        type->setBounds (getGridArea (0, 0));
        frequency->setBounds (getGridArea (1, 0));
        resonance->setBounds (getGridArea (2, 0));
        keyTracking->setBounds (getGridArea (0, 1));
        velocityTracking->setBounds (getGridArea (1, 1));
        amount->setBounds (getGridArea (2, 1));

        attack->setBounds (getGridArea (4, 0));
        decay->setBounds (getGridArea (5, 0));
        sustain->setBounds (getGridArea (4, 1));
        release->setBounds (getGridArea (5, 1));
    }

    ParamComponentPtr type, keyTracking, velocityTracking, frequency,
                      resonance, amount, attack, decay, sustain, release;
};

//==============================================================================
class ADSRBox : public gin::ControlBox
{
public:
    ADSRBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc)
        : gin::ControlBox (e)
    {
        auto& adsr = proc.adsrParams;

        add (attack           = new gin::Knob (adsr.attack));
        add (decay            = new gin::Knob (adsr.decay));
        add (sustain          = new gin::Knob (adsr.sustain));
        add (release          = new gin::Knob (adsr.release));

        add (velocityTracking = new gin::Knob (adsr.velocityTracking));
    }

    void resized() override
    {
        attack->setBounds (getGridArea (0, 0));
        decay->setBounds (getGridArea (1, 0));
        sustain->setBounds (getGridArea (0, 1));
        release->setBounds (getGridArea (1, 1));

        velocityTracking->setBounds (getGridArea (2, 0));
    }

    ParamComponentPtr velocityTracking, attack, decay, sustain, release;
};

//==============================================================================
class ModulationBox : public gin::ControlBox
{
public:
    ModulationBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc)
        : gin::ControlBox (e)
    {
        int cnt = 0;
        for (int i = 0; i < numElementsInArray (proc.lfoParams); i++, cnt++)
        {
            auto& env = proc.lfoParams[i];
            auto& pg  = lfos[i];

            add (cnt, pg.sync         = new gin::Switch (env.sync));
            add (cnt, pg.wave         = new gin::Select (env.wave));
            add (cnt, pg.rate         = new gin::Knob (env.rate));
            add (cnt, pg.beat         = new gin::Knob (env.beat));

            add (cnt, pg.depth        = new gin::Knob (env.depth));
            add (cnt, pg.phase        = new gin::Knob (env.phase));
            add (cnt, pg.offset       = new gin::Knob (env.offset));
            add (cnt, pg.fade         = new gin::Knob (env.fade));
            add (cnt, pg.delay        = new gin::Knob (env.delay));
        }

        for (int i = 0; i < numElementsInArray (proc.envParams); i++, cnt++)
        {
            auto& env = proc.envParams[i];
            auto& pg  = envs[i];

            add (cnt, pg.attack        = new gin::Knob (env.attack));
            add (cnt, pg.decay         = new gin::Knob (env.decay));
            add (cnt, pg.sustain       = new gin::Knob (env.sustain));
            add (cnt, pg.release       = new gin::Knob (env.release));
        }

        setPage (0);
    }

private:
    void resized() override
    {
        for (auto& l : lfos)
        {
            l.sync->setBounds (getGridArea (0, 0));
            l.wave->setBounds (getGridArea (1, 0));
            l.rate->setBounds (getGridArea (2, 0));
            l.beat->setBounds (getGridArea (3, 0));
            l.depth->setBounds (getGridArea (4, 0));
            l.phase->setBounds (getGridArea (1, 1));
            l.offset->setBounds (getGridArea (2, 1));
            l.fade->setBounds (getGridArea (3, 1));
            l.delay->setBounds (getGridArea (4, 1));
        }

        for (auto& e : envs)
        {
            e.attack->setBounds (getGridArea (0, 0));
            e.decay->setBounds (getGridArea (1, 0));
            e.sustain->setBounds (getGridArea (0, 1));
            e.release->setBounds (getGridArea (1, 1));
        }
    }

    struct LFO
    {
        ParamComponentPtr sync, wave, rate, beat, depth, phase, offset, fade, delay;
    };

    struct ENV
    {
        ParamComponentPtr attack, decay, sustain, release;
    };

    LFO lfos[2];
    ENV envs[2];
};

//==============================================================================
class EffectsBox : public gin::PagedControlBox
{
public:
    EffectsBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc)
        : gin::PagedControlBox (e)
    {
        int idx = 0;

        addPage ("Chorus", 3, 2);
        addPageEnable (idx, proc.chorusParams.enable);
        addControl (idx, new gin::Knob (proc.chorusParams.delay), 0, 0);
        addControl (idx, new gin::Knob (proc.chorusParams.rate), 1, 0);
        addControl (idx, new gin::Knob (proc.chorusParams.depth), 2, 0);
        addControl (idx, new gin::Knob (proc.chorusParams.width), 0, 1);
        addControl (idx, new gin::Knob (proc.chorusParams.mix), 1, 1);
        idx++;

        addPage ("Distortion", 2, 2);
        addPageEnable (idx, proc.distortionParams.enable);
        addControl (idx, new gin::Knob (proc.distortionParams.amount), 0, 0);
        addControl (idx, new gin::Knob (proc.distortionParams.highpass), 1, 0);
        addControl (idx, new gin::Knob (proc.distortionParams.output), 0, 1);
        addControl (idx, new gin::Knob (proc.distortionParams.mix), 1, 1);
        idx++;

        addPage ("EQ", 6, 2);
        addPageEnable (idx, proc.eqParams.enable);
        addControl (idx, new gin::Knob (proc.eqParams.loFreq), 0, 0);
        addControl (idx, new gin::Knob (proc.eqParams.loGain), 1, 0);
        addControl (idx, new gin::Knob (proc.eqParams.loQ), 2, 0);
        addControl (idx, new gin::Knob (proc.eqParams.mid1Freq), 3, 0);
        addControl (idx, new gin::Knob (proc.eqParams.mid1Gain), 4, 0);
        addControl (idx, new gin::Knob (proc.eqParams.mid1Q), 5, 0);
        addControl (idx, new gin::Knob (proc.eqParams.mid2Freq), 0, 1);
        addControl (idx, new gin::Knob (proc.eqParams.mid2Gain), 1, 1);
        addControl (idx, new gin::Knob (proc.eqParams.mid2Q), 2, 1);
        addControl (idx, new gin::Knob (proc.eqParams.hiFreq), 3, 1);
        addControl (idx, new gin::Knob (proc.eqParams.hiGain), 4, 1);
        addControl (idx, new gin::Knob (proc.eqParams.hiQ), 5, 1);
        idx++;

        addPage ("Comp", 3, 2);
        addPageEnable (idx, proc.compressorParams.enable);
        addControl (idx, new gin::Knob (proc.compressorParams.attack), 0, 0);
        addControl (idx, new gin::Knob (proc.compressorParams.release), 1, 0);
        addControl (idx, new gin::Knob (proc.compressorParams.ratio), 2, 0);
        addControl (idx, new gin::Knob (proc.compressorParams.threshold), 0, 1);
        addControl (idx, new gin::Knob (proc.compressorParams.gain), 1, 1);
        idx++;

        addPage ("Delay", 3, 2);
        addPageEnable (idx, proc.delayParams.enable);
        addControl (idx, new gin::Knob (proc.delayParams.sync), 0, 0);
        addControl (idx, new gin::Knob (proc.delayParams.time), 1, 0);
        addControl (idx, new gin::Knob (proc.delayParams.beat), 2, 0);
        addControl (idx, new gin::Knob (proc.delayParams.fb), 0, 1);
        addControl (idx, new gin::Knob (proc.delayParams.cf), 1, 1);
        addControl (idx, new gin::Knob (proc.delayParams.mix), 2, 1);
        idx++;

        addPage ("Reverb", 3, 2);
        addPageEnable (idx, proc.reverbParams.enable);
        addControl (idx, new gin::Knob (proc.reverbParams.damping), 0, 0);
        addControl (idx, new gin::Knob (proc.reverbParams.freezeMode), 1, 0);
        addControl (idx, new gin::Knob (proc.reverbParams.roomSize), 2, 0);
        addControl (idx, new gin::Knob (proc.reverbParams.width), 0, 1);
        addControl (idx, new gin::Knob (proc.reverbParams.mix), 1, 1);
        idx++;

        addPage ("Limiter", 2, 2);
        addPageEnable (idx, proc.limiterParams.enable);
        addControl (idx, new gin::Knob (proc.limiterParams.attack), 0, 0);
        addControl (idx, new gin::Knob (proc.limiterParams.release), 1, 0);
        addControl (idx, new gin::Knob (proc.limiterParams.threshold), 0, 1);
        addControl (idx, new gin::Knob (proc.limiterParams.gain), 1, 1);
        idx++;
    }
};
