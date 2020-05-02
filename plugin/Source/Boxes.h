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
    OscillatorBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc)
        : gin::PagedControlBox (e)
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
            addControl (i, new gin::Knob (osc.pulsewidth), 2, 1);
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
class FilterBox : public gin::PagedControlBox
{
public:
    FilterBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc)
        : gin::PagedControlBox (e)
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
            addControl (i, new gin::Knob (flt.velocityTracking), 1, 1);
            addControl (i, new gin::Knob (flt.amount), 2, 1);

            auto adsr = new gin::ADSRComponent ();
            adsr->setParams (flt.attack, flt.decay, flt.sustain, flt.release);
            addControl (i, adsr, 3, 0, 3, 2);

            addControl (i, new gin::Knob (flt.attack), 6, 0);
            addControl (i, new gin::Knob (flt.decay), 7, 0);
            addControl (i, new gin::Knob (flt.sustain), 6, 1);
            addControl (i, new gin::Knob (flt.release), 7, 1);
        }
    }
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
class ModulationBox : public gin::PagedControlBox
{
public:
    ModulationBox (gin::GinAudioProcessorEditor& e, VirtualAnalogAudioProcessor& proc)
        : gin::PagedControlBox (e)
    {
        int cnt = 0;
        for (int i = 0; i < numElementsInArray (proc.lfoParams); i++, cnt++)
        {
            auto& env = proc.lfoParams[i];

            addPage (i == 0 ? "LFO1" : "LFO2", 5, 2);

            addControl (cnt, new gin::Switch (env.sync), 0, 0);
            addControl (cnt, new gin::Select (env.wave), 1, 0);
            addControl (cnt, new gin::Knob (env.rate), 2, 0);
            addControl (cnt, new gin::Knob (env.beat), 3, 0);

            addControl (cnt, new gin::Knob (env.depth), 0, 1);
            addControl (cnt, new gin::Knob (env.phase), 1, 1);
            addControl (cnt, new gin::Knob (env.offset), 2, 1);
            addControl (cnt, new gin::Knob (env.fade), 3, 1);
            addControl (cnt, new gin::Knob (env.delay), 4, 1);
        }

        for (int i = 0; i < numElementsInArray (proc.envParams); i++, cnt++)
        {
            auto& env = proc.envParams[i];

            addPage (i == 0 ? "ENV1" : "ENV2", 2, 2);

            addControl (cnt, new gin::Knob (env.attack), 0, 0);
            addControl (cnt, new gin::Knob (env.decay), 1, 0);
            addControl (cnt, new gin::Knob (env.sustain), 0, 1);
            addControl (cnt, new gin::Knob (env.release), 1, 1);
        }
    }
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
