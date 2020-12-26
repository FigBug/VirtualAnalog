#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Panels.h"

//==============================================================================
class Editor : public Component
{
public:
    Editor (VirtualAnalogAudioProcessor& proc_)
        : proc ( proc_ )
    {
        for (auto& o : oscillators) addAndMakeVisible (o);
        for (auto& f : filters)     addAndMakeVisible (f);
        for (auto& a : adsrs)       addAndMakeVisible (a);
        for (auto& l : lfos)        addAndMakeVisible (l);
        for (auto& l : lfoGraphs)   addAndMakeVisible (l);

        addAndMakeVisible (mix);
        addAndMakeVisible (modulation);
        addAndMakeVisible (effects);

        for (int i = 0; i < Cfg::numLFOs; i++)
        {
            modulation.addBox (&lfos[i]);
            modulation.addBox (&lfoGraphs[i]);
        }

        effects.addBox (&gate);
        effects.addBox (&pattern);
        effects.addBox (&chorus);
        effects.addBox (&distort);
        effects.addBox (&eq);
        effects.addBox (&compress);
        effects.addBox (&delay);
        effects.addBox (&reverb);
        effects.addBox (&limit);
        effects.addBox (&scope);
    }

    void resized() override
    {
        auto rc = getLocalBounds();

        auto rcOsc = rc.removeFromTop (163);
        for (auto& o : oscillators) { o.setBounds (rcOsc.removeFromLeft (224)); rcOsc.removeFromLeft (1); };

        rc.removeFromTop (1);

        auto rcFlt = rc.removeFromTop (163);
        filters[0].setBounds (rcFlt.removeFromLeft (168));  rcFlt.removeFromLeft (1);
        adsrs[0].setBounds (rcFlt.removeFromLeft (186));    rcFlt.removeFromLeft (1);
        mix.setBounds (rcFlt.removeFromLeft (187));         rcFlt.removeFromLeft (1);
        adsrs[1].setBounds (rcFlt.removeFromLeft (186));    rcFlt.removeFromLeft (1);
        filters[1].setBounds (rcFlt.removeFromLeft (168));  rcFlt.removeFromLeft (1);

        //auto rcB1 = rc.removeFromTop (26);

        auto rcMod = rc.removeFromTop (163);
        modulation.setBounds (rcMod);

        //auto rcB2 = rc.removeFromTop (26);

        auto rcFX = rc.removeFromTop (163);
        effects.setBounds (rcFX);
    }

    VirtualAnalogAudioProcessor& proc;

    OscillatorBox oscillators[Cfg::numOSCs] { { "oscillator 1", proc, 0 }, { "oscillator 2", proc, 1 },
                                              { "oscillator 3", proc, 2 }, { "oscillator 4", proc, 3 } };

    FilterBox filters[Cfg::numFilters]      { { "filter 1", proc, 0 }, { "filter 2", proc, 1 } };

    ADSRArea adsrs[Cfg::numFilters]         { { proc, 0 }, { proc, 1 } };

    MixBox mix                              { "osc mix", proc };

    LFOBox lfos[Cfg::numLFOs]               { { "LFO 1", proc, 0 }, { "LFO 2", proc, 1 }, { "LFO 3", proc, 2 } };
    LFOArea lfoGraphs[Cfg::numLFOs]         { { proc, 0 }, { proc, 1 }, { proc, 2 } };

    GateBox gate { proc };
    GateArea pattern { proc };
    ChorusBox chorus { proc };
    DistortBox distort { proc };
    EQBox eq { proc };
    CompressBox compress { proc };
    DelayBox delay { proc };
    ReverbBox reverb { proc };
    LimitBox limit { proc };
    ScopeArea scope { proc };

    gin::BoxArea modulation;
    gin::BoxArea effects;
};

//==============================================================================
class VirtualAnalogAudioProcessorEditor : public gin::ProcessorEditor
{
public:
    VirtualAnalogAudioProcessorEditor (VirtualAnalogAudioProcessor&);
    ~VirtualAnalogAudioProcessorEditor() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    VirtualAnalogAudioProcessor& vaProc;

    Editor editor { vaProc };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualAnalogAudioProcessorEditor)
};
