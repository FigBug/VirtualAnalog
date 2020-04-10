#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "VirtualAnalogVoice.h"

using namespace gin;

//==============================================================================
void VirtualAnalogAudioProcessor::OSCParams::setup (VirtualAnalogAudioProcessor& p, int idx)
{
    String id = "osc" + String (idx + 1);
    String nm = "OSC" + String (idx + 1) + " ";

    wave       = p.addIntParam (id + "wave",       nm + "Wave",        "Wave",      "", { 0.0, 8.0, 1.0, 1.0 }, idx == 0 ? 2.0 : 0.0, {});
    voices     = p.addIntParam (id + "voices",     nm + "Voices",      "Voices",    "", { 1.0, 8.0, 1.0, 1.0 }, 1.0, {});
    tune       = p.addExtParam (id + "tune",       nm + "Tune",        "Tune",      "st", { -36.0, 36.0, 1.0, 1.0 }, 0.0, {});
    finetune   = p.addExtParam (id + "finetune",   nm + "Fine Tune",   "Fine Tune", "", { -100.0, 100.0, 0.0, 1.0 }, 0.0, {});
    level      = p.addExtParam (id + "level",      nm + "Level",       "Level",     "db", { -100.0, 0.0, 1.0, 4.0 }, 0.0, {});
    pulsewidth = p.addExtParam (id + "pulsewidth", nm + "Pulse Width", "PW",        "", { 1.0, 99.0, 0.0, 1.0 }, 50.0, {});
    detune     = p.addExtParam (id + "detune",     nm + "Detune",      "Detune",    "", { 0.0, 0.5, 0.0, 1.0 }, 0.0, {});
    spread     = p.addExtParam (id + "spread",     nm + "Spread",      "Spread",    "", { -100.0, 100.0, 0.0, 1.0 }, 0.0, {});
    pan        = p.addExtParam (id + "pan",        nm + "Pan",         "Pan",       "", { -1.0, 1.0, 0.0, 1.0 }, 0.0, {});
}

//==============================================================================
void VirtualAnalogAudioProcessor::FilterParams::setup (VirtualAnalogAudioProcessor& p, int idx)
{
    String id = "flt" + String (idx + 1);
    String nm = "FLT" + String (idx + 1) + " ";

    float maxFreq = float (getMidiNoteFromHertz (20000.0));

    type             = p.addIntParam (id + "type",    nm + "Type",    "Type",  "", { 0.0, 4.0, 1.0, 1.0 }, 0.0, {});
    slope            = p.addIntParam (id + "slope",   nm + "Slope",   "Slope", "", { 0.0, 1.0, 1.0, 1.0 }, 0.0, {});
    keyTracking      = p.addExtParam (id + "key",     nm + "Key",     "Key",   "", { 0.0, 100.0, 0.0, 1.0 }, 0.0, {});
    velocityTracking = p.addExtParam (id + "vel",     nm + "Vel",     "Vel",   "", { 0.0, 100.0, 0.0, 1.0 }, 0.0, {});
    frequency        = p.addExtParam (id + "freq",    nm + "Freq",    "Freq",  "", { 0.0, maxFreq, 0.0, 1.0 }, 0.0, {});
    resonance        = p.addExtParam (id + "res",     nm + "Res",     "Res",   "", { 0.0, 100.0, 0.0, 1.0 }, 0.0, {});
    amount           = p.addExtParam (id + "amount",  nm + "Amount",  "Amnt",  "", { -1.0, 1.0, 0.0, 1.0 }, 0.0, {});
    attack           = p.addExtParam (id + "attack",  nm + "Attack",  "A",     "", { 0.0, 60.0, 0.0, 0.2f }, 0.1f, {});
    decay            = p.addExtParam (id + "decay",   nm + "Decay",   "D",     "", { 0.0, 60.0, 0.0, 0.2f }, 0.1f, {});
    sustain          = p.addExtParam (id + "sustain", nm + "Sustain", "S",     "", { 0.0, 100.0, 0.0, 1.0 }, 80.0f, {});
    release          = p.addExtParam (id + "release", nm + "Release", "R",     "", { 0.0, 60.0, 0.0, 0.2f }, 0.1f, {});

    sustain->conversionFunction = [] (float in) { return in / 100.0f; };
}

//==============================================================================
void VirtualAnalogAudioProcessor::EnvParams::setup (VirtualAnalogAudioProcessor& p, int idx)
{
    String id = "env" + String (idx + 1);
    String nm = "ENV" + String (idx + 1) + " ";

    attack           = p.addExtParam (id + "attack",  nm + "Attack",  "A",     "", { 0.0, 60.0, 0.0, 0.2f }, 0.1f, {});
    decay            = p.addExtParam (id + "decay",   nm + "Decay",   "D",     "", { 0.0, 60.0, 0.0, 0.2f }, 0.1f, {});
    sustain          = p.addExtParam (id + "sustain", nm + "Sustain", "S",     "", { 0.0, 100.0, 0.0, 1.0 }, 80.0f, {});
    release          = p.addExtParam (id + "release", nm + "Release", "R",     "", { 0.0, 60.0, 0.0, 0.2f }, 0.1f, {});

    sustain->conversionFunction = [] (float in) { return in / 100.0f; };
}

//==============================================================================
VirtualAnalogAudioProcessor::VirtualAnalogAudioProcessor()
{
    synth.enableLegacyMode();
    synth.setVoiceStealingEnabled (true);

    for (int i = 0; i < numElementsInArray (oscParams); i++)
        oscParams[i].setup (*this, i);

    for (int i = 0; i < numElementsInArray (filterParams); i++)
        filterParams[i].setup (*this, i);

    for (int i = 0; i < numElementsInArray (envParams); i++)
        envParams[i].setup (*this, i);

    velocityTracking = addExtParam ("vel",     "Vel",     "Vel",   "", { 0.0, 100.0, 0.0, 1.0 }, 100.0, {});
    attack           = addExtParam ("attack",  "Attack",  "A",     "", { 0.0, 60.0, 0.0, 0.2f },  0.1f, {});
    decay            = addExtParam ("decay",   "Decay",   "D",     "", { 0.0, 60.0, 0.0, 0.2f },  0.1f, {});
    sustain          = addExtParam ("sustain", "Sustain", "S",     "", { 0.0, 100.0, 0.0, 1.0 }, 80.0f, {});
    release          = addExtParam ("release", "Release", "R",     "", { 0.0, 60.0, 0.0, 0.2f },  0.1f, {});

    sustain->conversionFunction = [] (float in) { return in / 100.0f; };

    for (int i = 0; i < 36; i++)
        synth.addVoice (new VirtualAnalogVoice (*this, bandLimitedLookupTables));
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
