/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EffectsChainAudioProcessor::EffectsChainAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif

{
}

EffectsChainAudioProcessor::~EffectsChainAudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout EffectsChainAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    //IIR parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("iir_cutoff", 1), "IIR Cutoff", 20.0f, 20000.0f, 1000.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("iir_bypass", 1), "IIR Bypass", false));
    
    //Reverb parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("reverb_size", 1), "Room Size", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("reverb_wet", 1), "Reverb Wet", 0.0f, 1.0f, 0.33f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("reverb_bypass", 1), "Reverb Bypass", false));
    
    //Distortion parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("dist_drive", 1), "Drive", 1.0f, 20.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("dist_bypass", 1), "Distortion Bypass", false));
    
    return { params.begin(), params.end() };
}

const juce::String EffectsChainAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EffectsChainAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EffectsChainAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EffectsChainAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EffectsChainAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EffectsChainAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EffectsChainAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EffectsChainAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EffectsChainAudioProcessor::getProgramName (int index)
{
    return {};
}

void EffectsChainAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EffectsChainAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    
    iirFilterL.prepare(spec);
    iirFilterR.prepare(spec);
    reverbL.setSampleRate(sampleRate);
    reverbR.setSampleRate(sampleRate);
    
}

void EffectsChainAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void EffectsChainAudioProcessor::processIIR(juce::AudioBuffer<float>& buffer)
{
    auto cutoff = apvts.getRawParameterValue("iir_cutoff") -> load();
    auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, cutoff);
    
    *iirFilterL.coefficients = *coefficients;
    *iirFilterR.coefficients = *coefficients;
    
    //Process left channel
    juce::dsp::AudioBlock<float> blockL(buffer.getArrayOfWritePointers(), 1, buffer.getNumSamples());
    juce::dsp::ProcessContextReplacing<float> contextL(blockL);
    iirFilterL.process(contextL);
    
    //Process right channel
    auto* rightChannel = buffer.getWritePointer(1);
    juce::dsp::AudioBlock<float> blockR(&rightChannel, 1, buffer.getNumSamples());
    juce::dsp::ProcessContextReplacing<float> contextR(blockR);
    iirFilterR.process(contextR);
}

void EffectsChainAudioProcessor::processReverb(juce::AudioBuffer<float> &buffer)
{
    juce::Reverb::Parameters reverbParams;
    reverbParams.roomSize = apvts.getRawParameterValue("reverb_size") -> load();
    reverbParams.wetLevel = apvts.getRawParameterValue("reverb_wet") -> load();
    reverbParams.dryLevel = 1.0f - reverbParams.wetLevel;
    
    reverbL.setParameters(reverbParams);
    reverbR.setParameters(reverbParams);
    
    reverbL.processMono(buffer.getWritePointer(0), buffer.getNumSamples());
    reverbR.processMono(buffer.getWritePointer(1), buffer.getNumSamples());
}

void EffectsChainAudioProcessor::processDistortion(juce::AudioBuffer<float> &buffer)
{
    float drive = apvts.getRawParameterValue("dist_drive") -> load();
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = std::tanh(channelData[sample] * drive);
        }
    }
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EffectsChainAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void EffectsChainAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    //Run effects in the order stored in effectOrder
    for (int slot = 0; slot < 3; ++slot)
    {
        int effectIndex = effectOrder[slot];
        
        if (effectIndex == 0 && !apvts.getRawParameterValue("iir_bypass")->load())
            processIIR(buffer);
        else if (effectIndex == 1 && !apvts.getRawParameterValue("reverb_bypass")->load())
            processReverb(buffer);
        else if (effectIndex == 2 && !apvts.getRawParameterValue("dist_bypass")->load())
            processDistortion(buffer);
    }
    
}

void EffectsChainAudioProcessor::setEffectOrder(int slot, int effectIndex)
{
    effectOrder[slot] = effectIndex;
}

//==============================================================================
bool EffectsChainAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EffectsChainAudioProcessor::createEditor()
{
    return new EffectsChainAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void EffectsChainAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void EffectsChainAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
/*juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EffectsChainAudioProcessor();
}*/
