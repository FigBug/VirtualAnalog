#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "VirtualAnalogVoice.h"

using namespace gin;

static String waveTextFunction (const Parameter&, float v)
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

String enableTextFunction (const Parameter&, float v)
{
    return v > 0.0f ? "On" : "Off";
}

String durationTextFunction (const Parameter&, float v)
{
    return NoteDuration::getNoteDurations()[size_t (v)].getName();
}

//==============================================================================
void VirtualAnalogAudioProcessor::OSCParams::setup (VirtualAnalogAudioProcessor& p, int idx)
{
    String id = "osc" + String (idx + 1);
    String nm = "OSC" + String (idx + 1) + " ";

    wave       = p.addIntParam (id + "wave",       nm + "Wave",        "Wave",      "", { 0.0, 7.0, 1.0, 1.0 }, idx == 0 ? 2.0 : 0.0, {}, waveTextFunction);
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
void VirtualAnalogAudioProcessor::GlobalParams::setup (VirtualAnalogAudioProcessor& p)
{
    mode    = p.addIntParam ("mode",    "Mode",     "",   "", { 0.0, 100.0, 0.0, 1.0 }, 100.0, {});
    legato  = p.addExtParam ("legato",  "Legato",   "",   "", { 0.0, 100.0, 0.0, 1.0 }, 100.0, {});
    level   = p.addExtParam ("level",   "Level",    "",   "db", { -100.0, 0.0, 1.0, 4.0 }, 0.0, {});
    voices  = p.addIntParam ("voices",  "Voices",   "",   "", { 0.0, 82.0, 1.0, 1.0 }, 32.0, {});
}

//==============================================================================
void VirtualAnalogAudioProcessor::ChorusParams::setup (VirtualAnalogAudioProcessor& p)
{
    enable = p.addIntParam ("chEnable",    "Enable",  "",   "", { 0.0, 1.0, 1.0, 1.0 }, 0.0, {});
    depth  = p.addExtParam ("chDepth",     "Depth",   "",   "ms", {0.1f, 20.0f, 0.0f, 1.0f}, 1.0f, {});
    speed  = p.addExtParam ("chSpeed",     "Speed",   "",   "Hz", {0.1f, 10.0f, 0.0f, 1.0f}, 3.0f, {});
    width  = p.addExtParam ("chWidth",     "Width",   "",   "", {0.0f, 1.0f,    0.0f, 1.0f}, 0.5f, {});
    mix    = p.addExtParam ("chMix",       "Mix",     "",   "", {0.0f, 1.0f,    0.0f, 1.0f}, 0.0f, {});
}

//==============================================================================
void VirtualAnalogAudioProcessor::DistortionParams::setup (VirtualAnalogAudioProcessor& p)
{
    enable = p.addIntParam ("dsEnable",    "Enable",     "",   "", { 0.0, 1.0, 1.0, 1.0 }, 0.0, {});
    amount = p.addExtParam ("dsAmount",    "Amount",     "",   "", { 0.0, 100.0, 0.0, 1.0 }, 0.0, {});
}

//==============================================================================
void VirtualAnalogAudioProcessor::EQParams::setup (VirtualAnalogAudioProcessor& p)
{
    float maxFreq = float (getMidiNoteFromHertz (20000.0));
    float d1 = float (getMidiNoteFromHertz (80.0));
    float d2 = float (getMidiNoteFromHertz (3000.0));
    float d3 = float (getMidiNoteFromHertz (5000.0));
    float d4 = float (getMidiNoteFromHertz (17000.0));

    enable   = p.addIntParam ("eqEnable",    "Enable",      "",     "", { 0.0, 1.0, 1.0, 1.0 }, 0.0, {});

    loFreq   = p.addExtParam ("eqLoFreq",    "Lo Freq",     "Freq", "", { 0.0, maxFreq, 0.0, 1.0 }, d1, {});
    loGain   = p.addExtParam ("eqLoQ",       "Lo Q",        "Q",    "", { 0.1f, 4.0f, 0.0, 1.0 }, 0.5f, {});
    loQ      = p.addExtParam ("eqLoGain",    "Lo Gain",     "Gain", "", { -20.0f, 20.0f, 0.0, 1.0 }, 0.0f, {});

    mid1Freq = p.addExtParam ("eqM1Freq",    "Min 1 Freq",  "Freq", "", { 0.0, maxFreq, 0.0, 1.0 }, d2, {});
    mid1Gain = p.addExtParam ("eqM1Q",       "Min 1 Q",     "Q",    "", { 0.1f, 4.0f, 0.0, 1.0 }, 0.5f, {});
    mid1Q    = p.addExtParam ("eqM1Gain",    "Min 1 Gain",  "Gain", "", { -20.0f, 20.0f, 0.0, 1.0 }, 0.0f, {});

    mid2Freq = p.addExtParam ("eqM2Freq",    "Mid 2 Freq",  "Freq", "", { 0.0, maxFreq, 0.0, 1.0 }, d3, {});
    mid2Gain = p.addExtParam ("eqM2Q",       "Mid 2 Q",     "Q",    "", { 0.1f, 4.0f, 0.0, 1.0 }, 0.5f, {});
    mid2Q    = p.addExtParam ("eqM2Gain",    "Mid 2 Gain",  "Gain", "", { -20.0f, 20.0f, 0.0, 1.0 }, 0.0f, {});

    hiFreq   = p.addExtParam ("eqHiFreq",    "Hi Freq",     "Freq", "", { 0.0, maxFreq, 0.0, 1.0 }, d4, {});
    hiGain   = p.addExtParam ("eqHiQ",       "Hi Q",        "Q",    "", { 0.1f, 4.0f, 0.0, 1.0 }, 0.5f, {});
    hiQ      = p.addExtParam ("eqHiGain",    "Hi Gain",     "Gain", "", { -20.0f, 20.0f, 0.0, 1.0 }, 0.0f, {});

    loFreq->conversionFunction   = [] (float in) { return Decibels::decibelsToGain (in); };
    mid1Freq->conversionFunction = [] (float in) { return Decibels::decibelsToGain (in); };
    mid2Freq->conversionFunction = [] (float in) { return Decibels::decibelsToGain (in); };
    hiFreq->conversionFunction   = [] (float in) { return Decibels::decibelsToGain (in); };
}

//==============================================================================
void VirtualAnalogAudioProcessor::CompressorParams::setup (VirtualAnalogAudioProcessor& p)
{
    enable    = p.addIntParam ("cpEnable",    "Enable",    "", "",     { 0.0, 1.0, 1.0, 1.0 }, 0.0, {});

    attack    = p.addExtParam ("cpAttack",    "Attack",    "", "ms",   { 1.0f,   200.0f, 0.0f, 0.4f},    1.0f, 0.1f);
    release   = p.addExtParam ("cpRelease",   "Release",   "", "ms",   { 1.0f,  2000.0f, 0.0f, 0.4f},    5.0f, 0.1f);
    ratio     = p.addExtParam ("cpRatio",     "Ratio",     "", "",     { 1.0f,    30.0f, 0.0f, 0.4f},    5.0f, 0.1f);
    threshold = p.addExtParam ("cpThreshold", "Threshold", "", "",     { -60.0f,   0.0f, 0.0f, 1.0f},  -30.0f, 0.1f);
    gain      = p.addExtParam ("cpGain",      "Gain",      "", "",     { -30.0f,  30.0f, 0.0f, 1.0f},    0.0f, 0.1f);

    attack->conversionFunction  = [] (float in) { return in / 1000.0; };
    release->conversionFunction = [] (float in) { return in / 1000.0; };
    gain->conversionFunction    = [] (float in) { return Decibels::decibelsToGain (in); };
}

//==============================================================================
void VirtualAnalogAudioProcessor::DelayParams::setup (VirtualAnalogAudioProcessor& p)
{
    enable = p.addIntParam ("rvEnable",    "Enable",     "",   "", { 0.0, 1.0, 1.0, 1.0 }, 0.0, {});

    float mxd = float (NoteDuration::getNoteDurations().size()) - 1.0f;

    sync  = p.addExtParam ("dlSync",  "Sync",      "", "",   {   0.0f,   1.0f, 1.0f, 1.0f},    0.0f, 0.0f, enableTextFunction);
    time  = p.addExtParam ("dlTime",  "Delay",     "", "",   {   0.0f, 120.0f, 0.0f, 0.3f},    1.0f, 0.0f);
    beat  = p.addExtParam ("dlBeat",  "Delay",     "", "",   {   0.0f,    mxd, 1.0f, 1.0f},   13.0f, 0.0f, durationTextFunction);
    fb    = p.addExtParam ("dlFb",    "Feedback",  "", "dB", {-100.0f,   0.0f, 0.0f, 5.0f},  -10.0f, 0.1f);
    cf    = p.addExtParam ("dlCf",    "Crossfeed", "", "dB", {-100.0f,   0.0f, 0.0f, 5.0f}, -100.0f, 0.1f);
    mix   = p.addExtParam ("dlMix",   "Mix",       "", "%",  {   0.0f, 100.0f, 0.0f, 1.0f},    0.0f, 0.1f);

    delay = p.addIntParam ("dlDelay", "Delay",     "", "",   {   0.0f, 120.0f, 0.0f, 1.0f},    1.0f, {0.2f, SmoothingType::eased});

    fb->conversionFunction  = [] (float in) { return Decibels::decibelsToGain (in); };
    cf->conversionFunction  = [] (float in) { return Decibels::decibelsToGain (in); };
    mix->conversionFunction = [] (float in) { return in / 100.0f; };
}

//==============================================================================
void VirtualAnalogAudioProcessor::ReverbParams::setup (VirtualAnalogAudioProcessor& p)
{
    enable = p.addIntParam ("rvEnable",    "Enable",     "",   "", { 0.0, 1.0, 1.0, 1.0 }, 0.0, {});
}

//==============================================================================
void VirtualAnalogAudioProcessor::LimiterParams::setup (VirtualAnalogAudioProcessor& p)
{
    enable    = p.addIntParam ("lmEnable",    "Enable",    "", "",     { 0.0, 1.0, 1.0, 1.0 }, 0.0, {});

    attack    = p.addExtParam ("lmAttack",    "Attack",    "", "ms",   { 1.0f,     5.0f, 0.0f, 0.4f},    1.0f, 0.1f);
    release   = p.addExtParam ("lmRelease",   "Release",   "", "ms",   { 1.0f,   100.0f, 0.0f, 0.4f},    5.0f, 0.1f);
    threshold = p.addExtParam ("lmThreshold", "Threshold", "", "",     { -60.0f,   0.0f, 0.0f, 1.0f},  -30.0f, 0.1f);
    gain      = p.addExtParam ("lmGain",      "Gain",      "", "",     { -30.0f,  30.0f, 0.0f, 1.0f},    0.0f, 0.1f);
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
    globalParams.setup (*this);
    chorusParams.setup (*this);
    distortionParams.setup (*this);
    eqParams.setup (*this);
    compressorParams.setup (*this);
    delayParams.setup (*this);
    reverbParams.setup (*this);
    limiterParams.setup (*this);

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

    stereoDelay.setSampleRate (sampleRate);
    compressor.setSampleRate (sampleRate);
    limiter.setSampleRate (sampleRate);

    eq.setNumChannels (2);
    eq.setSampleRate (sampleRate);

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

        auto slice = sliceBuffer (buffer, pos, thisBlock);
        applyEffects (slice);

        modMatrix.finishBlock (thisBlock);
        
        pos += thisBlock;
        todo -= thisBlock;
    }
    
    playHead = nullptr;
}

void VirtualAnalogAudioProcessor::applyEffects (AudioSampleBuffer& buffer)
{
    // Apply Chorus
    
    // Apply Distortion

    // Apply EQ
    if (eqParams.enable->isOn())
        eq.process (buffer);

    // Apply Compressor
    if (compressorParams.enable->isOn())
        compressor.process (buffer);

    // Apply Delay
    if (delayParams.enable->isOn())
        stereoDelay.process (buffer);

    // Apply Reverb

    // Apply Limiter
    if (limiterParams.enable->isOn())
        limiter.process (buffer);
}

void VirtualAnalogAudioProcessor::updateParams (int blockSize)
{
    // Update Mono LFOs
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

    // Update Chorus
    // Update Distortion

    // Update EQ
    if (eqParams.enable->isOn())
    {
        eq.setParams (0, EQ::lowshelf,
                      modMatrix.getValue (eqParams.loFreq),
                      modMatrix.getValue (eqParams.loQ),
                      modMatrix.getValue (eqParams.loGain));
    }

    // Update Compressor
    if (compressorParams.enable->isOn())
    {
        compressor.setInputGain (1.0f);
        compressor.setOutputGain (modMatrix.getValue (compressorParams.gain));
        compressor.setParams (modMatrix.getValue (compressorParams.attack),
                              modMatrix.getValue (compressorParams.release),
                              modMatrix.getValue (compressorParams.threshold),
                              modMatrix.getValue (compressorParams.ratio),
                              6);
    }

    // Update Delay
    if (delayParams.enable->isOn())
    {
        if (delayParams.sync->isOn())
        {
            auto& duration = NoteDuration::getNoteDurations()[(size_t)delayParams.beat->getUserValueInt()];
            delayParams.delay->setUserValue (duration.toSeconds (getPlayHead()));
        }
        else
        {
            delayParams.delay->setUserValue (delayParams.time->getUserValue());
        }

        stereoDelay.setParams (modMatrix.getValue (delayParams.delay),
                               modMatrix.getValue (delayParams.mix),
                               modMatrix.getValue (delayParams.fb),
                               modMatrix.getValue (delayParams.cf));
    }


    // Update Reverb

    // Update Limiter
    if (limiterParams.enable->isOn())
    {
        limiter.setInputGain (1.0f);
        limiter.setOutputGain (modMatrix.getValue (limiterParams.gain));
        limiter.setParams (modMatrix.getValue (limiterParams.attack),
                           modMatrix.getValue (limiterParams.release),
                           modMatrix.getValue (limiterParams.threshold),
                           100, 6);
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
