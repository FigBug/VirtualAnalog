#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace gin;

//==============================================================================
VirtualAnalogAudioProcessorEditor::VirtualAnalogAudioProcessorEditor (VirtualAnalogAudioProcessor& p)
    : ProcessorEditor (p, 50, 50 + 15), proc (p)
{
    addAndMakeVisible (editor);

    setSize (901, 753);
}

VirtualAnalogAudioProcessorEditor::~VirtualAnalogAudioProcessorEditor()
{
}

//==============================================================================
void VirtualAnalogAudioProcessorEditor::paint (Graphics& g)
{
    ProcessorEditor::paint (g);

    g.fillAll (findColour (PluginLookAndFeel::blackColourId));
}

void VirtualAnalogAudioProcessorEditor::resized()
{
    ProcessorEditor::resized ();

    auto rc = getLocalBounds().reduced (1);
    rc.removeFromTop (40);

    editor.setBounds (rc);
}
