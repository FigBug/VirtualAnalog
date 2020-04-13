#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class HeaderBox : public gin::ControlBox
{
public:
    HeaderBox (VirtualAnalogAudioProcessor& proc)
    {
        for (int i = 0; i < 4; i++)
        {
            auto osc = proc.oscParams[i];

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
            detune[i]->setBounds (getGridArea (2, 0));
            spread[i]->setBounds (getGridArea (3, 0));
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
    OscillatorBox (VirtualAnalogAudioProcessor& proc, int idx)
    {
        auto osc = proc.oscParams[idx];

        add (wave       = new gin::Select (osc.wave));
        add (tune       = new gin::Knob (osc.tune));
        add (finetune   = new gin::Knob (osc.finetune));
        add (pan        = new gin::Knob (osc.pan));
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
class VirtualAnalogAudioProcessorEditor : public gin::GinAudioProcessorEditor
{
public:
    VirtualAnalogAudioProcessorEditor (VirtualAnalogAudioProcessor&);
    ~VirtualAnalogAudioProcessorEditor() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    VirtualAnalogAudioProcessor& proc;

    HeaderBox header {proc};
    OscillatorBox oscillators[4] =
    {
        { proc, 0 },
        { proc, 1 },
        { proc, 2 },
        { proc, 3 },
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualAnalogAudioProcessorEditor)
};
