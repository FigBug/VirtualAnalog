#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace gin;

//==============================================================================
VirtualAnalogAudioProcessorEditor::VirtualAnalogAudioProcessorEditor (VirtualAnalogAudioProcessor& p)
    : ProcessorEditor (p, 50, 50 + 15), proc (p)
{
    oscHeaders.addChildComponent (modOverview);

    gin::addAndMakeVisible (*this, { &commonHeader, &common, &unisonHeader });

    gin::addAndMakeVisible (*this, { &oscHeaders });
    gin::addAndMakeVisible (*this, { &oscillators });

    gin::addAndMakeVisible (*this, { &filterHeaders });
    gin::addAndMakeVisible (*this, { &filters });

    gin::addAndMakeVisible (*this, { &adsrHeader, &adsr });
    gin::addAndMakeVisible (*this, { &modulationHeader, &modulation });
    gin::addAndMakeVisible (*this, { &effectsHeader, &effects });

    setGridSize (14, 8, 0, 4 * 25);
}

VirtualAnalogAudioProcessorEditor::~VirtualAnalogAudioProcessorEditor()
{
}

//==============================================================================
void VirtualAnalogAudioProcessorEditor::paint (Graphics& g)
{
    ProcessorEditor::paint (g);

    g.setColour (Colours::white.withAlpha (0.2f));

    auto rc = getFullGridArea();
    g.drawRect (rc.expanded (1));
}

void VirtualAnalogAudioProcessorEditor::resized()
{
    ProcessorEditor::resized ();

    auto rc = getFullGridArea();

    int hh = 25;

    int gx = getGridWidth();
    int gy = getGridHeight();

    // Oscillators
    {
        auto rHeaders = rc.removeFromTop (hh);
        oscHeaders.setBounds (rHeaders.removeFromLeft (gx * 14));

        auto rOscs = rc.removeFromTop (gy * 2);
        oscillators.setBounds (rOscs.removeFromLeft (gx * 14));

        modOverview.setBounds (4, 4, 200, hh - 8);
    }

    // Filters
    {
        auto rHeaders = rc.removeFromTop (hh);
        filterHeaders.setBounds (rHeaders.removeFromLeft (gx * 14));

        auto rFilters = rc.removeFromTop (gy * 2);
        filters.setBounds (rFilters.removeFromLeft (gx * 14));
    }

    // ADSR and mod
    {
        auto rHeaders = rc.removeFromTop (hh);
        adsrHeader.setBounds (rHeaders.removeFromLeft (gx * 6));
        modulationHeader.setBounds (rHeaders.removeFromLeft (gx * 8));

        auto rFilters = rc.removeFromTop (gy * 2);
        adsr.setBounds (rFilters.removeFromLeft (gx * 6));
        modulation.setBounds (rFilters.removeFromLeft (gx * 8));
    }

    // Effects & Common
    {
        auto rHeaders = rc.removeFromTop (hh);
        effectsHeader.setBounds (rHeaders.removeFromLeft (gx * 10));
        commonHeader.setBounds (rHeaders.removeFromLeft (gx * 4));

        auto rControls = rc.removeFromTop (gy * 2);
        effects.setBounds (rControls.removeFromLeft (gx * 10));
        common.setBounds (rControls.removeFromLeft (gx * 4));
    }
}
