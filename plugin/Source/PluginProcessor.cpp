#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "VirtualAnalogVoice.h"

using namespace gin;

static String textFunction (const Parameter&, float v)
{
    switch ((Wave)int (v))
    {
        case Wave::silence:     return "Off";
        case Wave::sine:        return "Sine";
        case Wave::triangle:    return "Triangle";
        case Wave::sawUp:       return "Saw (Up)";
        case Wave::sawDown:     return "Saw (Down)";
        case Wave::pulse:       return "Pulse";
        case Wave::square:      return "Square";
        case Wave::noise:       return "Noise";
    }
}

//==============================================================================
void VirtualAnalogAudioProcessor::OSCParams::setup (VirtualAnalogAudioProcessor& p, int idx)
{
    String id = "osc" + String (idx + 1);
    String nm = "OSC" + String (idx + 1) + " ";

    wave       = p.addIntParam (id + "wave",       nm + "Wave",        "Wave",      "", { 0.0, 7.0, 1.0, 1.0 }, idx == 0 ? 2.0 : 0.0, {}, textFunction);
    voices     = p.addIntParam (id + "voices",     nm + "Voices",      "Voices",    "", { 1.0, 8.0, 1.0, 1.0 }, 1.0, {});
    tune       = p.addExtParam (id + "tune",       nm + "Tune",        "Tune",      "st", { -36.0, 36.0, 1.0, 1.0 }, 0.0, {});
    finetune   = p.addExtParam (id + "finetune",   nm + "Fine Tune",   "Fine",      "", { -100.0, 100.0, 0.0, 1.0 }, 0.0, {});
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
void VirtualAnalogAudioProcessor::LFOParams::setup (VirtualAnalogAudioProcessor& p, int idx)
{
    String id = "lfo" + String (idx + 1);
    String nm = "LFO" + String (idx + 1) + " ";
    
    auto notes = NoteDuration::getNoteDurations();

    sync             = p.addIntParam (id + "sync",    nm + "Sync",    "Sync",   "", { 0.0, 1.0, 1.0, 1.0 }, 0.0, {});
    wave             = p.addIntParam (id + "wave",    nm + "Wave",    "Wave",   "", { 0.0, 12.0, 1.0, 1.0 }, 0.0, {});
    rate             = p.addExtParam (id + "rate",    nm + "Rate",    "Rate",   "", { 0.0, 500.0, 0.0, 0.3f }, 10.0, {});
    beat             = p.addExtParam (id + "beat",    nm + "Beat",    "Beat",   "", { 0.0, float (notes.size() - 1), 1.0, 1.0 }, 0.0, {});
    depth            = p.addExtParam (id + "depth",   nm + "Depth",   "Depth",  "", { 0.0, 1.0, 0.0, 1.0 }, 1.0, {});
    phase            = p.addExtParam (id + "phase",   nm + "Phase",   "Phase",  "", { 0.0, 1.0, 0.0, 1.0 }, 0.0, {});
    offset           = p.addExtParam (id + "offset",  nm + "Offset",  "Offset", "", { -1.0, 1.0, 0.0, 1.0 }, 0.0, {});
    fade             = p.addExtParam (id + "fade",    nm + "Fade",    "Fade",   "", { 0.0, 60.0, 0.0, 1.0 }, 0.1f, {});
    delay            = p.addExtParam (id + "delay",   nm + "Delay",   "Delay",  "", { 0.0, 60.0, 0.0, 1.0 }, 0.1f, {});
}

//==============================================================================
void VirtualAnalogAudioProcessor::ADSRParams::setup (VirtualAnalogAudioProcessor& p)
{
    velocityTracking = p.addExtParam ("vel",     "Vel",     "Vel",   "", { 0.0, 100.0, 0.0, 1.0 }, 100.0, {});
    attack           = p.addExtParam ("attack",  "Attack",  "A",     "", { 0.0, 60.0, 0.0, 0.2f },  0.1f, {});
    decay            = p.addExtParam ("decay",   "Decay",   "D",     "", { 0.0, 60.0, 0.0, 0.2f },  0.1f, {});
    sustain          = p.addExtParam ("sustain", "Sustain", "S",     "", { 0.0, 100.0, 0.0, 1.0 }, 80.0f, {});
    release          = p.addExtParam ("release", "Release", "R",     "", { 0.0, 60.0, 0.0, 0.2f },  0.1f, {});

    sustain->conversionFunction = [] (float in) { return in / 100.0f; };
}

//==============================================================================
VirtualAnalogAudioProcessor::VirtualAnalogAudioProcessor()
{
    enableLegacyMode();
    setVoiceStealingEnabled (true);

    for (int i = 0; i < numElementsInArray (oscParams); i++)
        oscParams[i].setup (*this, i);

    for (int i = 0; i < numElementsInArray (filterParams); i++)
        filterParams[i].setup (*this, i);

    for (int i = 0; i < numElementsInArray (envParams); i++)
        envParams[i].setup (*this, i);

    for (int i = 0; i < numElementsInArray (lfoParams); i++)
        lfoParams[i].setup (*this, i);

    adsrParams.setup (*this);

    for (int i = 0; i < 36; i++)
    {
        auto voice = new VirtualAnalogVoice (*this, bandLimitedLookupTables);
        modMatrix.addVoice (voice);
        addVoice (voice);
    }

    setupModMatrix();
}

VirtualAnalogAudioProcessor::~VirtualAnalogAudioProcessor()
{
}

//==============================================================================
void VirtualAnalogAudioProcessor::setupModMatrix()
{
    modSrcPressure  = modMatrix.addPolyModSource ("mpePressure");
    modSrcTimbre    = modMatrix.addPolyModSource ("mpeTimbre");

    modScrPitchBend = modMatrix.addMonoModSource ("pitchBend");

    modSrcNote      = modMatrix.addPolyModSource ("note");
    modSrcVelocity  = modMatrix.addPolyModSource ("velocity");

    for (int i = 0; i <= 119; i++)
        modSrcCC[i] = modMatrix.addMonoModSource (String::formatted ("cc%d", i));

    for (int i = 0; i < VirtualAnalogVoice::numLFOs; i++)
        modSrcMonoLFO[i] = modMatrix.addMonoModSource (String::formatted ("mlfo%d", i));

    for (int i = 0; i < VirtualAnalogVoice::numLFOs; i++)
        modSrcLFO[i] = modMatrix.addPolyModSource (String::formatted ("lfo%d", i));

    for (int i = 0; i < VirtualAnalogVoice::numFilters; i++)
        modSrcFilter[i] = modMatrix.addPolyModSource (String::formatted ("fenv%d", i));

    for (int i = 0; i < VirtualAnalogVoice::numENVs; i++)
        modSrcEvn[i] = modMatrix.addPolyModSource (String::formatted ("env%d", i));

    for (auto pp : getPluginParameters())
        if (! pp->isInternal())
            modMatrix.addParameter (pp);

    modMatrix.build();
}

void VirtualAnalogAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    GinProcessor::prepareToPlay (sampleRate, samplesPerBlock);
    
    bandLimitedLookupTables.setSampleRate (sampleRate);
    setCurrentPlaybackSampleRate (sampleRate);

    modMatrix.setSampleRate (sampleRate);

    for (auto& l : modLFOs)
        l.setSampleRate (sampleRate);
}

void VirtualAnalogAudioProcessor::releaseResources()
{
}

void VirtualAnalogAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    ScopedNoDenormals noDenormals;
    playhead = getPlayHead();
    
    int pos = 0;
    int todo = buffer.getNumSamples();
    
    buffer.clear();
    
    while (todo > 0)
    {
        int thisBlock = std::min (todo, 32);

        updateParams (thisBlock);
        renderNextBlock (buffer, midi, pos, thisBlock);
        modMatrix.finishBlock (thisBlock);
        
        pos += thisBlock;
        todo -= thisBlock;
    }
    
    playHead = nullptr;
}

void VirtualAnalogAudioProcessor::updateParams (int blockSize)
{
    for (int i = 0; i < VirtualAnalogVoice::numLFOs; i++)
    {
        LFO::Parameters params;

        float freq = 0;
        if (lfoParams[i].sync->getProcValue() > 0.0f)
            freq = 1.0f / NoteDuration::getNoteDurations()[size_t (lfoParams[i].beat->getProcValue())].toSeconds (playhead);
        else
            freq = modMatrix.getValue (lfoParams[i].rate);

        params.waveShape = (LFO::WaveShape) int (lfoParams[i].wave->getProcValue());
        params.frequency = freq;
        params.phase     = modMatrix.getValue (lfoParams[i].phase);
        params.offset    = modMatrix.getValue (lfoParams[i].offset);
        params.depth     = modMatrix.getValue (lfoParams[i].depth);
        params.delay     = 0;
        params.fade      = 0;

        modLFOs[i].setParameters (params);
        modLFOs[i].process (blockSize);
    }
}

void VirtualAnalogAudioProcessor::handleMidiEvent (const MidiMessage& m)
{
	MPESynthesiser::handleMidiEvent (m);
	
    if (m.isPitchWheel())
        modMatrix.setMonoValue (modScrPitchBend, float (m.getPitchWheelValue()) / 0x2000 - 1.0f);
}

void VirtualAnalogAudioProcessor::handleController ([[maybe_unused]] int ch, int num, int val)
{
    modMatrix.setMonoValue (modSrcCC[num], val / 127.0f);
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
