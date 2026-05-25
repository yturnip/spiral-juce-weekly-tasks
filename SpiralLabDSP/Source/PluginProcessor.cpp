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
                       )
#endif
{
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
    // initialisation that you need..
    const int maxDelayMs      = 50;
    const int maxDelaySamples = (int) (sampleRate * maxDelayMs / 1000.0);

    // Delay
    delay.prepare (sampleRate, maxDelaySamples);
    delay.setDelaySamples ((int) (sampleRate * 0.5)); // 300 ms
    delay.setFeedback (0.5f);
    delay.setWet (0.5f);

    // Vibrato
    vibrato.prepare (sampleRate, maxDelaySamples);
    vibrato.setFrequency (5.0f);
    vibrato.setDepthSamples (sampleRate * 0.003f); // ~3 ms swing

    // Flanger
    flanger.prepare (sampleRate, maxDelaySamples);
    flanger.setFrequency (0.5f);
    flanger.setSweepSamples (sampleRate * 0.003f); // ~3 ms
    flanger.setDepth (0.7f);
    flanger.setFeedback (0.4f);

    // Chorus: 3-voice
    const int numVoices = 3;
    chorus.prepare (sampleRate, maxDelaySamples, numVoices);
    chorus.setFrequency (0.5f);
    chorus.setBaseDelaySamples (sampleRate * 0.02f);   // ~20 ms
    chorus.setSweepSamples (sampleRate * 0.005f);      // ~5 ms
    chorus.setDepth (0.8f);
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
    int numSamples = buffer.getNumSamples();
    
    if (totalNumInputChannels > 0)
    {
        auto* channelData = buffer.getWritePointer(0);
        
        for (int i = 0; i < numSamples; ++i)
        {
            float x = channelData[i];

            switch (currentEffect)
            {
                case EffectType::Delay:
                    x = delay.processSample (x);
                    break;

                case EffectType::Vibrato:
                    x = vibrato.processSample (x);
                    break;

                case EffectType::Flanger:
                    x = flanger.processSample (x);
                    break;

                case EffectType::Chorus:
                    x = chorus.processSample (x);
                    break;
            }

            channelData[i] = x;
        }
    }
    
    for (int ch = 1; ch < totalNumOutputChannels; ++ch)
        buffer.copyFrom(ch, 0, buffer, 0, 0, numSamples);
}

//==============================================================================
bool SpiralLabDSPAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SpiralLabDSPAudioProcessor::createEditor()
{
    return new SpiralLabDSPAudioProcessorEditor (*this);
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
