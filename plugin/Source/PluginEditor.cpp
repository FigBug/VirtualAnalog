#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace gin;

//==============================================================================
VirtualAnalogAudioProcessorEditor::VirtualAnalogAudioProcessorEditor (VirtualAnalogAudioProcessor& p)
    : GinAudioProcessorEditor (p, 60, 100), proc (p)
{
    addAndMakeVisible (header);
    addAndMakeVisible (oscillators[0]);
    addAndMakeVisible (oscillators[1]);
    addAndMakeVisible (oscillators[2]);
    addAndMakeVisible (oscillators[3]);

    setGridSize (12, 7);
}

VirtualAnalogAudioProcessorEditor::~VirtualAnalogAudioProcessorEditor()
{
}

//==============================================================================
void VirtualAnalogAudioProcessorEditor::paint (Graphics& g)
{
    GinAudioProcessorEditor::paint (g);
}

void VirtualAnalogAudioProcessorEditor::resized()
{
    GinAudioProcessorEditor::resized ();

    header.setBounds (getGridArea (0, 0, 12, 1));
    oscillators[0].setBounds (getGridArea (0, 1, 3, 2));
    oscillators[1].setBounds (getGridArea (3, 1, 3, 2));
    oscillators[2].setBounds (getGridArea (6, 1, 3, 2));
    oscillators[3].setBounds (getGridArea (9, 1, 3, 2));
}
