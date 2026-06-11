/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpiralLabDSPAudioProcessor::SpiralLabDSPAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
apvts (*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
SpiralLabDSPAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    /*
    //Tremolo
    params.push_back (std::make_unique<juce::AudioParameterChoice>(
        "tremWaveform",
        "Tremolo Waveform",
        juce::StringArray { "Sine", "Triangle", "Square", "Square Sloped Edges" },
        0
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "tremFreq",
        "Tremolo Frequency (Hz)",
        juce::NormalisableRange<float> (0.1f, 20.0f),
        5.0f
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "tremDepth",
        "Tremolo Depth",
        juce::NormalisableRange<float> (0.0f, 1.0f),
        0.5f
    ));
    */
    
    /*
    //Ring Modulation
    params.push_back (std::make_unique<juce::AudioParameterChoice>(
        "ringWaveform",
        "RingMod Waveform",
        juce::StringArray { "Sine", "Triangle", "Square", "Square Sloped Edges" },
        0
    ));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "ringFreq",
        "RingMod Frequency (Hz)",
        juce::NormalisableRange<float> (10.0f, 2000.0f, 0.01f, 0.3f), // skewed range
        200.0f
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "ringDepth",
        "RingMod Depth",
        juce::NormalisableRange<float> (0.0f, 1.0f),
        1.0f
    ));
     */
    
    /*
    //Compressor Parameters
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "compThresh",
        "Compressor Threshold (dB)",
        juce::NormalisableRange<float> (-60.0f, 0.0f),
        0.0f
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "compRatio",
        "Compressor Ratio",
        juce::NormalisableRange<float> (1.0f, 20.0f),
        2.0f
    ));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "compAttack",
        "Compressor Attack (ms)",
        juce::NormalisableRange<float> (0.1f, 200.0f),
        10.0f
    ));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "compRelease",
        "Compressor Release (ms)",
        juce::NormalisableRange<float> (10.0f, 2000.0f),
        100.0f
    ));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "compMakeUp",
        "Compressor Make Up (dB)",
        juce::NormalisableRange<float> (-24.0f, 24.0f),
        0.0f
    ));
    */
    
    /*
    //Expander Parameters
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "expThresh",
        "Expander Threshold (dB)",
        juce::NormalisableRange<float> (-60.0f, 0.0f),
        0.0f
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "expRatio",
        "Expander Ratio",
        juce::NormalisableRange<float> (1.0f, 20.0f),
        2.0f
    ));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "expAttack",
        "Expander Attack (ms)",
        juce::NormalisableRange<float> (0.1f, 200.0f),
        10.0f
    ));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "expRelease",
        "Expander Release (ms)",
        juce::NormalisableRange<float> (10.0f, 2000.0f),
        100.0f
    ));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "expFloor",
        "Expander Floor (dB)",
        juce::NormalisableRange<float> (-80.0f, 0.0f),
        -80.0f
    ));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "expMakeUp",
        "Expander Make Up (dB)",
        juce::NormalisableRange<float> (-24.0f, 24.0f),
        0.0f
    ));
    */
    
    
    params.push_back (std::make_unique<juce::AudioParameterChoice>(
        "distType",
        "Distortion Type",
        juce::StringArray { "Hard Clipping", "Soft Clipping", "Soft Clipping Exponential", "Full Wave Rectifier", "Half Wave Rectifier"},
        0
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "distInputGain",
        "Input Gain (dB)",
        juce::NormalisableRange<float> (-0.0f, 40.0f),
        0.0f
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "distOutputGain",
        "Output Gain (dB)",
        juce::NormalisableRange<float> (-40.0f, 0.0f),
        0.0f
    ));

    return { params.begin(), params.end() };
}

void SpiralLabDSPAudioProcessor::pushInputToScope (const float* monoData, int numSamples)
{
    if (auto* ed = dynamic_cast<SpiralLabDSPAudioProcessorEditor*> (getActiveEditor()))
        ed->pushInputSamples (monoData, numSamples);
}

void SpiralLabDSPAudioProcessor::pushOutputToScope (const float* monoData, int numSamples)
{
    if (auto* ed = dynamic_cast<SpiralLabDSPAudioProcessorEditor*> (getActiveEditor()))
        ed->pushOutputSamples (monoData, numSamples);
}

SpiralLabDSPAudioProcessor::~SpiralLabDSPAudioProcessor()
{
}

//==============================================================================
const juce::String SpiralLabDSPAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SpiralLabDSPAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SpiralLabDSPAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SpiralLabDSPAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SpiralLabDSPAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SpiralLabDSPAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SpiralLabDSPAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SpiralLabDSPAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SpiralLabDSPAudioProcessor::getProgramName (int index)
{
    return {};
}

void SpiralLabDSPAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SpiralLabDSPAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need.
    
    juce::ignoreUnused(samplesPerBlock);
    
    const float fs = static_cast<float> (sampleRate);

    // Tremolo
    trem.setSampleRate (fs);
    trem.reset();
    
    // Ring Mod
    ring.setSampleRate (fs);
    ring.reset();

    // Compressor
    comp.setSampleRate (fs);
    comp.reset();

    // Expander
    exp.setSampleRate (fs);
    exp.reset();
    
    dist.setSampleRate(fs);
    dist.reset();
}

void SpiralLabDSPAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SpiralLabDSPAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SpiralLabDSPAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples             = buffer.getNumSamples();
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    /*
    // Tremolo
    {
        int   wfIndex   = (int) *apvts.getRawParameterValue ("tremWaveform");
        float tremFreq  =      *apvts.getRawParameterValue ("tremFreq");
        float tremDepth =      *apvts.getRawParameterValue ("tremDepth");
        
        trem.setFrequency (tremFreq);
        trem.setDepth (tremDepth);
        switch (wfIndex)
        {
            case 0: trem.setWaveform (SpiralTremolo::Sine); break;
            case 1: trem.setWaveform (SpiralTremolo::Triangle); break;
            case 2: trem.setWaveform (SpiralTremolo::Square); break;
            case 3: trem.setWaveform (SpiralTremolo::SquareSlopedEdges); break;
            default: break;
        }
    }
     */
    
    /*
    // Ring Mod
    {
        int   wfIndex   = (int) *apvts.getRawParameterValue ("ringWaveform");
        float ringFreq  =      *apvts.getRawParameterValue ("ringFreq");
        float ringDepth =      *apvts.getRawParameterValue ("ringDepth");
        
        ring.setCarrierFrequency (ringFreq);
        ring.setDepth (ringDepth);
        switch (wfIndex)
        {
            case 0: ring.setWaveform (SpiralRingMod::Sine); break;
            case 1: ring.setWaveform (SpiralRingMod::Triangle); break;
            case 2: ring.setWaveform (SpiralRingMod::Square); break;
            case 3: ring.setWaveform (SpiralRingMod::SquareSlopedEdges); break;
            default: break;
        }
    }
     */
    
    /*
    const float thresh   = *apvts.getRawParameterValue ("compThresh");
    const float ratio    = *apvts.getRawParameterValue ("compRatio");
    const float attackMs = *apvts.getRawParameterValue ("compAttack");
    const float relMs    = *apvts.getRawParameterValue ("compRelease");
    const float makeUp   = *apvts.getRawParameterValue ("compMakeUp");
    
    comp.setThreshold(thresh);
    comp.setRatio(ratio);
    comp.setAttack(attackMs);
    comp.setRelease(relMs);
    comp.setMakeUpGain(makeUp);
    */
    
    /*
    const float thresh   = *apvts.getRawParameterValue ("expThresh");
    const float ratio    = *apvts.getRawParameterValue ("expRatio");
    const float attackMs = *apvts.getRawParameterValue ("expAttack");
    const float relMs    = *apvts.getRawParameterValue ("expRelease");
    const float floor    = *apvts.getRawParameterValue ("expFloor");
    const float makeUp   = *apvts.getRawParameterValue ("expMakeUp");
    
    exp.setThreshold(thresh);
    exp.setRatio(ratio);
    exp.setAttack(attackMs);
    exp.setRelease(relMs);
    exp.setFloorGain(floor);
    exp.setMakeUpGain(makeUp);
    */
    
    // Distortion
    {
        int   dtIndex   = (int) *apvts.getRawParameterValue ("distType");
        float inputGain  =      *apvts.getRawParameterValue ("distInputGain");
        float outputGain =      *apvts.getRawParameterValue ("distOutputGain");
        
        dist.setInputGainDb(inputGain);
        dist.setOutputGainDb(outputGain);
        switch (dtIndex)
        {
            case 0: dist.setDistortionType (SpiralDistortion::HardClipping); break;
            case 1: dist.setDistortionType (SpiralDistortion::SoftClipping); break;
            case 2: dist.setDistortionType (SpiralDistortion::SoftClippingExponential); break;
            case 3: dist.setDistortionType (SpiralDistortion::FullWaveRectifier); break;
            case 4: dist.setDistortionType (SpiralDistortion::HalfWaveRectifier); break;
            default: break;
        }
    }
    
    auto* left  = buffer.getWritePointer (0);
    auto* right = totalNumOutputChannels > 1 ? buffer.getWritePointer (1) : nullptr;
    
    juce::HeapBlock<float> inputMono (numSamples);
    juce::HeapBlock<float> outputMono (numSamples);
    
    for (int i = 0; i < numSamples; ++i)
    {
        float inL = left[i];
        float inR = (right != nullptr ? right[i] : inL);
        
        const float monoIn = 0.5f * (inL + inR);
        
        inputMono[i] = monoIn;
        
        /*
        // Tremolo
        inL = trem.processSample (inL);
        inR = trem.processSample (inR);
         */
        
        /*
        // Ring Mod
        inL = ring.processSample (inL);
        inR = ring.processSample (inR);
         */
        
        /*
        // Compressor gain
        const float gComp = comp.computeGain (monoIn);
        float postCompL = inL * gComp;
        float postCompR = inR * gComp;
        */
        
        /*
        // Expander gain
        const float gExp = exp.computeGain (monoIn);
        float postExpL = inL * gExp;
        float postExpR = inR * gExp;
        */
        
        // Distortion
        inL = dist.processSample (inL);
        inR = dist.processSample (inR);
        
        const float outL = inL;
        const float outR = (right != nullptr ? inR : outL);

        left[i] = outL;
        if (right != nullptr)
            right[i] = outR;
        
        outputMono[i] = 0.5f * (outL + outR);
    }
    
    pushInputToScope  (inputMono.get(),  numSamples);
    pushOutputToScope (outputMono.get(), numSamples);
}

//==============================================================================
bool SpiralLabDSPAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SpiralLabDSPAudioProcessor::createEditor()
{
    return new SpiralLabDSPAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void SpiralLabDSPAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SpiralLabDSPAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpiralLabDSPAudioProcessor();
}
