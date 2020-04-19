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

    gin::addAndMakeVisible (*this, { &filterHeaders[0], &filterHeaders[1] });
    gin::addAndMakeVisible (*this, { &filters[0], &filters[1] });

    gin::addAndMakeVisible (*this, { &adsrHeader, &adsr });
    gin::addAndMakeVisible (*this, { &modulationHeader, &modulation });
    gin::addAndMakeVisible (*this, { &effectsHeader, &effects });

    setGridSize (12, 8, 0, 5 * 25);
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

    // Common and Unison box
    {
        auto rHeaders = rc.removeFromTop (hh);
        auto rControls = rc.removeFromTop (cy);

        commonHeader.setBounds (rHeaders.removeFromLeft (cx * 8));
        common.setBounds (rControls.removeFromLeft (cx * 8));

        unisonHeader.setBounds (rHeaders.removeFromLeft (cx * 4));
        unison.setBounds (rControls.removeFromLeft (cx * 4));
    }

    // Oscillators
    {
        auto rHeaders = rc.removeFromTop (hh);
        for (auto& c : oscHeaders)
            c.setBounds (rHeaders.removeFromLeft (cx * 3));

        auto rOscs = rc.removeFromTop (cy * 2);
        for (auto& c : oscillators)
            c.setBounds (rOscs.removeFromLeft (cx * 3));
    }

    // Filters
    {
        auto rHeaders = rc.removeFromTop (hh);
        for (auto& c : filterHeaders)
            c.setBounds (rHeaders.removeFromLeft (cx * 6));

        auto rFilters = rc.removeFromTop (cy * 2);
        for (auto& c : filters)
            c.setBounds (rFilters.removeFromLeft (cx * 6));
    }

    // ADSR and mod
    {
        auto rHeaders = rc.removeFromTop (hh);
        adsrHeader.setBounds (rHeaders.removeFromLeft (cx * 3));
        modulationHeader.setBounds (rHeaders.removeFromLeft (cx * 9));

        auto rFilters = rc.removeFromTop (cy * 2);
        adsr.setBounds (rFilters.removeFromLeft (cx * 3));
        modulation.setBounds (rFilters.removeFromLeft (cx * 9));
    }

    // Effects
    {
        auto rHeaders = rc.removeFromTop (hh);
        effectsHeader.setBounds (rHeaders.removeFromLeft (cx * 12));

        auto rFilters = rc.removeFromTop (cy * 1);
        effects.setBounds (rFilters.removeFromLeft (cx * 12));
    }
}
