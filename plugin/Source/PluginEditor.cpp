#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VirtualAnalogAudioProcessorEditor::VirtualAnalogAudioProcessorEditor (VirtualAnalogAudioProcessor& p)
    : ProcessorEditor (p, 50, 50 + 15), vaProc (p)
{
    setLookAndFeel (&lf);
    addAndMakeVisible (editor);

    setSize (901, 753);
}

VirtualAnalogAudioProcessorEditor::~VirtualAnalogAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void VirtualAnalogAudioProcessorEditor::paint (juce::Graphics& g)
{
    ProcessorEditor::paint (g);

    g.fillAll (findColour (gin::PluginLookAndFeel::blackColourId));
}

void VirtualAnalogAudioProcessorEditor::resized()
{
    ProcessorEditor::resized ();

    auto rc = getLocalBounds().reduced (1);
    rc.removeFromTop (40);

    editor.setBounds (rc);
}
