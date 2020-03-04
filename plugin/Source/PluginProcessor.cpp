#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace gin;

//==============================================================================
VirtualAnalogVoice::VirtualAnalogVoice()
{
}

void VirtualAnalogVoice::noteStarted()
{
    adsr.noteOn();
}

void VirtualAnalogVoice::noteStopped (bool allowTailOff)
{
    adsr.noteOff();
    
    if (! allowTailOff)
        clearCurrentNote();
}

void VirtualAnalogVoice::setCurrentSampleRate (double newRate)
{
    MPESynthesiserVoice::setCurrentSampleRate (newRate);
    
    adsr.setSampleRate (newRate);
}

void VirtualAnalogVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    ScratchBuffer buffer (2, numSamples);
    
    // Run ADSR
    ScratchBuffer adsrOutput (1, numSamples);
    adsr.process (adsrOutput);
    
    if (adsr.getState() == AnalogADSR::State::idle)
        clearCurrentNote();
    
    // Copy output to synth
    outputBuffer.copyFrom (0, startSample, buffer, 0, 0, numSamples);
    outputBuffer.copyFrom (1, startSample, buffer, 1, 0, numSamples);
}

//==============================================================================
VirtualAnalogAudioProcessor::VirtualAnalogAudioProcessor()
{
    setVoiceStealingEnabled (true);
    
    for (int i = 0; i < 32; i++)
        addVoice (new VirtualAnalogVoice());
}

VirtualAnalogAudioProcessor::~VirtualAnalogAudioProcessor()
{
}

//==============================================================================
void VirtualAnalogAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    GinProcessor::prepareToPlay (sampleRate, samplesPerBlock);
    
    setCurrentPlaybackSampleRate (sampleRate);
}

void VirtualAnalogAudioProcessor::releaseResources()
{
}

void VirtualAnalogAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    
    int pos = 0;
    int todo = buffer.getNumSamples();
    
    while (todo > 0)
    {
        int thisBlock = std::min (todo, 32);
        renderNextBlock (buffer, midiMessages, pos, thisBlock);
        
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
