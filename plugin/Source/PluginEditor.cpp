#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace gin;

//==============================================================================
VirtualAnalogAudioProcessorEditor::VirtualAnalogAudioProcessorEditor (VirtualAnalogAudioProcessor& p)
    : GinAudioProcessorEditor (p, 60, 100), proc (p)
{
    for (auto pp : p.getPluginParameters())
    {
        ParamComponent* pc;

        if (pp->isOnOff())
            pc = new Switch (pp);
        else
            pc = new Knob (pp);

        addAndMakeVisible (pc);
        controls.add (pc);
    }

    setGridSize (9, 7);
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

    int idx = 0;
    for (auto pp : proc.getPluginParameters())
    {
        componentForParam (*pp)->setBounds (getGridArea (idx % 9, idx / 9));
        idx++;
    }
}
