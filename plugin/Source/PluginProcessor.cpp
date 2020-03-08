#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "VirtualAnalogVoice.h"

using namespace gin;

//==============================================================================
VirtualAnalogAudioProcessor::VirtualAnalogAudioProcessor()
{
    synth.enableLegacyMode();
    synth.setVoiceStealingEnabled (true);
    
    for (int i = 0; i < 32; i++)
        synth.addVoice (new VirtualAnalogVoice (*this));
}

VirtualAnalogAudioProcessor::~VirtualAnalogAudioProcessor()
{
}

//==============================================================================
void VirtualAnalogAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    GinProcessor::prepareToPlay (sampleRate, samplesPerBlock);
    
    bandLimitedLookupTables.setSampleRate (sampleRate);
    synth.setCurrentPlaybackSampleRate (sampleRate);
}

void VirtualAnalogAudioProcessor::releaseResources()
{
}

void VirtualAnalogAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    ScopedNoDenormals noDenormals;
    
    int pos = 0;
    int todo = buffer.getNumSamples();
    
    buffer.clear();
    
    while (todo > 0)
    {
        int thisBlock = std::min (todo, 32);
        synth.renderNextBlock (buffer, midi, pos, thisBlock);
        
        pos += thisBlock;
        todo -= thisBlock;
    }
}

//==============================================================================
bool VirtualAnalogAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* VirtualAnalogAudioProcessor::createEditor()
{
    return new VirtualAnalogAudioProcessorEditor (*this);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VirtualAnalogAudioProcessor();
}
