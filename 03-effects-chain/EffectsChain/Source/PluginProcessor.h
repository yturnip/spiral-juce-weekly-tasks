/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class EffectsChainAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    EffectsChainAudioProcessor();
    ~EffectsChainAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //APVTS
    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    //Effect order (stores indices 0=IIR, 1=Reverb, 2=Distortion)
    std::array<int, 3> effectOrder { 0, 1, 2};
    void setEffectOrder(int slot, int effectIndex);

private:
    juce::dsp::IIR::Filter<float> iirFilterL, iirFilterR;
    juce::Reverb reverbL, reverbR;
    
    void applyDistortion(juce::AudioBuffer<float>& buffer, float drive);
    
    void processIIR(juce::AudioBuffer<float>& buffer);
    void processReverb(juce::AudioBuffer<float>& buffer);
    void processDistortion(juce::AudioBuffer<float>& buffer);
    
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectsChainAudioProcessor)
};
