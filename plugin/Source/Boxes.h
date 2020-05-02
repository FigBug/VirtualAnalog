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
        add (level = new gin::Knob (proc.globalParams.level));
    }

    void resized() override
    {
        level->setBounds (getGridArea (0, 0));
    }

    ParamComponentPtr level;
};

//==============================================================================
class OscillatorBox : public gin::PagedControlBox
{
public:
    OscillatorBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc_)
        : gin::PagedControlBox (e), proc (proc_)
    {
        for ( int i = 0; i < numElementsInArray (proc.oscParams); i++)
        {
            auto& osc = proc.oscParams[i];
            
            addPage ("OSC " + String (i + 1), 3, 2);
            addPageEnable (i, osc.enable);

            addControl (i, new gin::Select (osc.wave), 0, 0);
            addControl (i, new gin::Knob (osc.tune, true), 1, 0);
            addControl (i, new gin::Knob (osc.finetune, true), 2, 0);
            addControl (i, new gin::Knob (osc.pan, true), 0, 1);
            addControl (i, new gin::Knob (osc.level), 1, 1);
            addControl (i, pw[i] = new gin::Knob (osc.pulsewidth), 2, 1);

            watchParam (osc.wave);
        }
        
        addPage ("Unison", 2, 2);
        for (int i = 0; i < 4; i++)
        {
            auto& osc = proc.oscParams[i];

            addControl (4, voices[i] = new gin::Knob (osc.voices), 1, 0);
            addControl (4, detune[i] = new gin::Knob (osc.detune), 0, 1);
            addControl (4, spread[i] = new gin::Knob (osc.spread), 1, 1);
        }
        setOSC (0);
    }

    void paramChanged () override
    {
        gin::PagedControlBox::paramChanged ();

        for ( int i = 0; i < numElementsInArray (proc.oscParams); i++)
        {
            auto& osc = proc.oscParams[i];
            pw[i]->setEnabled ((gin::Wave) int (osc.wave->getProcValue()) == gin::Wave::pulse);
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

    VirtualAnalogAudioProcessor& proc;
    ParamComponentPtr pw[4], voices[4], detune[4], spread[4];
};

//==============================================================================
class FilterBox : public gin::PagedControlBox
{
public:
    FilterBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc_)
        : gin::PagedControlBox (e), proc (proc_)
    {
        for (int i = 0; i < numElementsInArray (proc.filterParams); i++)
        {
            auto& flt = proc.filterParams[i];
            
            addPage ("Filter " + String (i + 1), 8, 2);
            addPageEnable (i, flt.enable);

            addControl (i, new gin::Select (flt.type), 0, 0);
            addControl (i, new gin::Knob (flt.frequency), 1, 0);
            addControl (i, new gin::Knob (flt.resonance), 2, 0);

            addControl (i, new gin::Knob (flt.keyTracking), 0, 1);
            addControl (i, new gin::Knob (flt.amount), 1, 1);
            addControl (i, v[i] = new gin::Knob (flt.velocityTracking), 2, 1);

            adsr[i] = new gin::ADSRComponent ();
            adsr[i]->setParams (flt.attack, flt.decay, flt.sustain, flt.release);
            addControl (i, adsr[i], 3, 0, 3, 2);

            addControl (i, a[i] = new gin::Knob (flt.attack), 6, 0);
            addControl (i, d[i] = new gin::Knob (flt.decay), 7, 0);
            addControl (i, s[i] = new gin::Knob (flt.sustain), 6, 1);
            addControl (i, r[i] = new gin::Knob (flt.release), 7, 1);

            watchParam (flt.amount);
        }
    }

    void paramChanged () override
    {
        gin::PagedControlBox::paramChanged ();

        for ( int i = 0; i < numElementsInArray (proc.filterParams); i++)
        {
            auto& flt = proc.filterParams[i];
            v[i]->setEnabled (flt.amount->getUserValue() != 0.0f);
            a[i]->setEnabled (flt.amount->getUserValue() != 0.0f);
            d[i]->setEnabled (flt.amount->getUserValue() != 0.0f);
            s[i]->setEnabled (flt.amount->getUserValue() != 0.0f);
            r[i]->setEnabled (flt.amount->getUserValue() != 0.0f);
            adsr[i]->setEnabled (flt.amount->getUserValue() != 0.0f);
        }
    }

    VirtualAnalogAudioProcessor& proc;
    ParamComponentPtr v[2], a[2], d[2], s[2], r[2];
    gin::ADSRComponent* adsr[2];
};

//==============================================================================
class ADSRBox : public gin::ControlBox
{
public:
    ADSRBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc)
        : gin::ControlBox (e)
    {
        auto& env = proc.adsrParams;

        add (attack           = new gin::Knob (env.attack));
        add (decay            = new gin::Knob (env.decay));
        add (sustain          = new gin::Knob (env.sustain));
        add (release          = new gin::Knob (env.release));

        add (velocityTracking = new gin::Knob (env.velocityTracking));

        adsr = new gin::ADSRComponent ();
        adsr->setParams (env.attack, env.decay, env.sustain, env.release);
        add (adsr);
    }

    void resized() override
    {
        attack->setBounds (getGridArea (0, 0));
        decay->setBounds (getGridArea (1, 0));
        sustain->setBounds (getGridArea (0, 1));
        release->setBounds (getGridArea (1, 1));

        velocityTracking->setBounds (getGridArea (2, 0));

        adsr->setBounds (getGridArea (3, 0, 3, 2));
    }

    ParamComponentPtr velocityTracking, attack, decay, sustain, release;
    gin::ADSRComponent* adsr;
};

//==============================================================================
class ModulationBox : public gin::PagedControlBox
{
public:
    ModulationBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc_)
        : gin::PagedControlBox (e), proc (proc_)
    {
        int cnt = 0;
        for (int i = 0; i < numElementsInArray (proc.lfoParams); i++, cnt++)
        {
            auto& env = proc.lfoParams[i];

            addPage (i == 0 ? "LFO1" : "LFO2", 7, 2);

            addControl (cnt, new gin::Select (env.wave), 1, 0);
            addControl (cnt, new gin::Switch (env.sync), 0, 0);
            addControl (cnt, r[i] = new gin::Knob (env.rate), 2, 0);
            addControl (cnt, b[i] = new gin::Select (env.beat), 2, 0);
            addControl (cnt, new gin::Knob (env.depth), 3, 0);

            addControl (cnt, new gin::Knob (env.phase), 0, 1);
            addControl (cnt, new gin::Knob (env.offset), 1, 1);
            addControl (cnt, new gin::Knob (env.fade), 2, 1);
            addControl (cnt, new gin::Knob (env.delay), 3, 1);

            watchParam (env.sync);
        }

        for (int i = 0; i < numElementsInArray (proc.envParams); i++, cnt++)
        {
            auto& env = proc.envParams[i];

            addPage (i == 0 ? "ENV1" : "ENV2", 5, 2);

            addControl (cnt, new gin::Knob (env.attack), 0, 0);
            addControl (cnt, new gin::Knob (env.decay), 1, 0);
            addControl (cnt, new gin::Knob (env.sustain), 0, 1);
            addControl (cnt, new gin::Knob (env.release), 1, 1);

            auto adsr = new gin::ADSRComponent ();
            adsr->setParams (env.attack, env.decay, env.sustain, env.release);
            addControl (cnt, adsr, 2, 0, 3, 2);
        }
    }

    void paramChanged () override
    {
        gin::PagedControlBox::paramChanged ();

        for (int i = 0; i < numElementsInArray (proc.lfoParams); i++)
        {
            auto& lfo = proc.lfoParams[i];
            r[i]->setVisible (! lfo.sync->isOn());
            b[i]->setVisible (lfo.sync->isOn());
        }
    }

    VirtualAnalogAudioProcessor& proc;
    ParamComponentPtr r[2], b[2];
};

//==============================================================================
class EffectsBox : public gin::PagedControlBox
{
public:
    EffectsBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc_)
        : gin::PagedControlBox (e), proc (proc_)
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
        addControl (idx, new gin::Switch (proc.delayParams.sync), 0, 0);
        addControl (idx, t = new gin::Knob (proc.delayParams.time), 1, 0);
        addControl (idx, b = new gin::Select (proc.delayParams.beat), 1, 0);
        addControl (idx, new gin::Knob (proc.delayParams.fb), 2, 0);
        addControl (idx, new gin::Knob (proc.delayParams.cf), 1, 1);
        addControl (idx, new gin::Knob (proc.delayParams.mix), 2, 1);
        idx++;

        watchParam (proc.delayParams.sync);

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

    void paramChanged () override
    {
        gin::PagedControlBox::paramChanged ();

        t->setVisible (! proc.delayParams.sync->isOn());
        b->setVisible (proc.delayParams.sync->isOn());
    }

    VirtualAnalogAudioProcessor& proc;
    ParamComponentPtr t, b;
};
