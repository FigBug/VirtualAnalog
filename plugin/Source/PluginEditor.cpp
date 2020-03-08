#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace gin;

//==============================================================================
VirtualAnalogAudioProcessorEditor::VirtualAnalogAudioProcessorEditor (VirtualAnalogAudioProcessor& p)
    : GinAudioProcessorEditor (p), proc (p)
{
    setGridSize (5, 3);
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
}
