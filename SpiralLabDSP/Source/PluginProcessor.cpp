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
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
    "randcomb_wipe",
    "RandComb Wipe",
    juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
    0.0f));
    
    params.push_back (std::make_unique<juce::AudioParameterBool> (
    "randcomb_trigger",
    "RandComb Trigger",
    false));
     */
    
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
    "randwipe_wipe",
    "RandWipe Wipe",
    juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
    0.0f));
    
    params.push_back (std::make_unique<juce::AudioParameterBool> (
    "randwipe_trigger",
    "RandWipe Trigger",
    false));
    
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
    /*
    const int convBlockSize = samplesPerBlock;
    
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    juce::File irFile ("/Users/MsiModern/SPIRALab/spiral-juce-weekly-tasks/SpiralLabDSP/Source/small-room-ir-pack/252847__kijjaz__20141025-kijjaz-ir-new-office-01.aiff");

    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (irFile));
    if (reader != nullptr)
    {
        // --- Resample IR from its native rate to the session sample rate ---
        juce::AudioBuffer<float> rawIR ((int) reader->numChannels, (int) reader->lengthInSamples);
        reader->read (&rawIR, 0, (int) reader->lengthInSamples, 0, true, true);
        
        const double sourceSampleRate = reader->sampleRate;
        const double ratio = sampleRate / sourceSampleRate; // e.g. 44100 / 192000

        const int resampledLength = juce::roundToInt (rawIR.getNumSamples() * ratio);
        
        juce::AudioBuffer<float> resampledIR (1, resampledLength);
        resampledIR.clear();

        // Use JUCE's LagrangeInterpolator for quality resampling
        juce::LagrangeInterpolator resampler;
        resampler.reset();

        const float* src = rawIR.getReadPointer (0);
        float* dst = resampledIR.getWritePointer (0);

        // speedRatio = sourceSampleRate / destSampleRate (how many source samples per dest sample)
        const double speedRatio = sourceSampleRate / sampleRate;
        resampler.process (speedRatio, src, dst, resampledLength);

        testIR.resize (resampledLength);
        for (int i = 0; i < resampledLength; ++i)
            testIR[i] = dst[i];
    }
    else
    {
        testIR.assign (samplesPerBlock, 0.0f);
        testIR[0] = 1.0f;
    }
    
    if (!testIR.empty())
    {
        // Compute RMS energy of the IR
        float energy = 0.0f;
        for (float s : testIR)
            energy += s * s;

        energy = std::sqrt (energy / (float) testIR.size());

        // Avoid division by zero
        if (energy > 1e-6f)
        {
            const float gain = 1.0f / energy;
            for (float& s : testIR)
                s *= gain;
        }
    }
    
    rvb.prepare(sampleRate, convBlockSize, testIR);
    */
    /*
    randComb.setSampleRate (sampleRate);
    randComb.prepare (1024, 256);
     */
    randWipe.setSampleRate ((float) sampleRate);
    randWipe.prepare  (1024, 256);   // A path
    randWipe.prepareB (1024, 256);   // B path
    randWipe.trigger();
    
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    juce::File bFile ("/Users/MsiModern/Downloads/mondamusic-guitar-solo-guitar-music-499187.mp3");
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (bFile));

    if (reader != nullptr)
    {
        const int len = (int) reader->lengthInSamples;
        sourceBBuffer.setSize (1, len);
        reader->read (&sourceBBuffer, 0, len, 0, true, false);
        DBG ("Source B loaded: " << len << " samples");
    }
    else
    {
        // Fallback: white noise buffer of 1 second
        const int len = (int) sampleRate;
        sourceBBuffer.setSize (1, len);
        for (int i = 0; i < len; ++i)
            sourceBBuffer.setSample (0, i, ((float) rand() / RAND_MAX) * 2.0f - 1.0f);
        DBG ("Source B file not found — using noise fallback");
    }

    sourceBReadPos = 0;
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
    
    auto* left  = buffer.getWritePointer (0);
    auto* right = totalNumOutputChannels > 1 ? buffer.getWritePointer (1) : nullptr;
    /*
    float wipeVal = apvts.getRawParameterValue ("randcomb_wipe")->load();
    randComb.setWipe (wipeVal);
    
    bool triggerVal = apvts.getRawParameterValue ("randcomb_trigger")->load() > 0.5f;
    if (triggerVal && !lastTriggerState)
        randComb.trigger();
    lastTriggerState = triggerVal;
    
    juce::HeapBlock<float> inputMono (numSamples);
    juce::HeapBlock<float> outputMono (numSamples);
    
    for (int i = 0; i < numSamples; ++i)
    {
        float inL = left[i];
        float inR = (right != nullptr ? right[i] : inL);
     
        const float monoIn = 0.5f * (inL + inR);
     
        inputMono[i] = monoIn;
    }
    
    //rvb.processBlock(inputMono.getData(), outputMono.getData(), numSamples);
    randComb.processBlock (inputMono.getData(), numSamples);
        
    const float dryWet = 0.0f;
    for (int i = 0; i < numSamples; i++)
    {
        float dry = inputMono[i];
        float wet = outputMono[i];
        float y   = (1.0f - dryWet) * dry + dryWet * wet;
        
        left[i] = y;
        if (right != nullptr)
            right[i] = y;
    }
     */
    float wipeVal   = apvts.getRawParameterValue ("randwipe_wipe")->load();
    bool triggerVal = apvts.getRawParameterValue ("randwipe_trigger")->load() > 0.5f;

    randWipe.setWipe (wipeVal);

    if (triggerVal && !lastTriggerState)
        randWipe.trigger();
    lastTriggerState = triggerVal;

    juce::HeapBlock<float> sourceA (numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        float inL = left[i];
        float inR = (right != nullptr) ? right[i] : inL;
        sourceA[i] = 0.5f * (inL + inR);
    }

    juce::HeapBlock<float> sourceB (numSamples);
    const int bLen = sourceBBuffer.getNumSamples();
    for (int i = 0; i < numSamples; ++i)
    {
        sourceB[i] = (bLen > 0) ? sourceBBuffer.getSample (0, sourceBReadPos) : 0.0f;
        if (bLen > 0 && ++sourceBReadPos >= bLen)
            sourceBReadPos = 0;
    }
    
    randWipe.processBSource (sourceB.getData(), numSamples);
    randWipe.processBlock   (sourceA.getData(), numSamples);
    
    for (int i = 0; i < numSamples; ++i)
    {
        left[i] = sourceA[i];
        if (right != nullptr)
            right[i] = sourceA[i];
    }
    
    //pushInputToScope(inputMono.get(), numSamples);
    //pushOutputToScope (outputMono.get(), numSamples);
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
