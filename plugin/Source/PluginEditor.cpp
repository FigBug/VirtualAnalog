#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace gin;

//==============================================================================
VirtualAnalogAudioProcessorEditor::VirtualAnalogAudioProcessorEditor (VirtualAnalogAudioProcessor& p)
    : GinAudioProcessorEditor (p, 50, 50 + 15), proc (p)
{
    gin::addAndMakeVisible (*this, { &commonHeader, &common, &unisonHeader, &unison });

    gin::addAndMakeVisible (*this, { &oscHeaders[0], &oscHeaders[1], &oscHeaders[2], &oscHeaders[3] });
    gin::addAndMakeVisible (*this, { &oscillators[0], &oscillators[1], &oscillators[2], &oscillators[3] });

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
    GinAudioProcessorEditor::paint (g);

    g.setColour (Colours::white.withAlpha (0.2f));

    auto rc = getFullGridArea();
    g.drawRect (rc.expanded (1));
}

void VirtualAnalogAudioProcessorEditor::resized()
{
    GinAudioProcessorEditor::resized ();

    auto rc = getFullGridArea();

    int hh = 25;

    int cx = getGridWidth();
    int cy = getGridHeight();

    // Oscillators
    {
        auto rHeaders = rc.removeFromTop (hh);
        for (auto& c : oscHeaders)
            c.setBounds (rHeaders.removeFromLeft (cx * 3));

        unisonHeader.setBounds (rHeaders.removeFromLeft (cx * 2));

        auto rOscs = rc.removeFromTop (cy * 2);
        for (auto& c : oscillators)
            c.setBounds (rOscs.removeFromLeft (cx * 3));

        unison.setBounds (rOscs.removeFromLeft (cx * 2));
    }

    // Filters
    {
        auto rHeaders = rc.removeFromTop (hh);
        filterHeaders.setBounds (rHeaders.removeFromLeft (cx * 14));

        auto rFilters = rc.removeFromTop (cy * 2);
        filters.setBounds (rFilters.removeFromLeft (cx * 14));
    }

    // ADSR and mod
    {
        auto rHeaders = rc.removeFromTop (hh);
        adsrHeader.setBounds (rHeaders.removeFromLeft (cx * 5));
        modulationHeader.setBounds (rHeaders.removeFromLeft (cx * 9));

        auto rFilters = rc.removeFromTop (cy * 2);
        adsr.setBounds (rFilters.removeFromLeft (cx * 5));
        modulation.setBounds (rFilters.removeFromLeft (cx * 9));
    }

    // Effects & Common
    {
        auto rHeaders = rc.removeFromTop (hh);
        effectsHeader.setBounds (rHeaders.removeFromLeft (cx * 10));
        commonHeader.setBounds (rHeaders.removeFromLeft (cx * 4));

        auto rControls = rc.removeFromTop (cy * 2);
        effects.setBounds (rControls.removeFromLeft (cx * 10));
        common.setBounds (rControls.removeFromLeft (cx * 4));
    }
}
