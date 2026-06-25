/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "dsp/SpiralDelay.h"
#include "dsp/SpiralVibrato.h"
#include "dsp/SpiralFlanger.h"
#include "dsp/SpiralChorus.h"
#include "dsp/SpiralOnePoleLowPass.h"
#include "dsp/SpiralBiquad.h"
#include "dsp/SpiralWahWah.h"
#include "dsp/SpiralTremolo.h"
#include "dsp/SpiralRingMod.h"
#include "dsp/SpiralCompressor.h"
#include "dsp/SpiralExpanderGate.h"
#include "dsp/SpiralDistortion.h"
#include "dsp/SpiralPhaseVocoder.h"
#include "dsp/SpiralComb.h"
#include "dsp/SpiralAllpassSchroeder.h"
#include "dsp/SpiralConvolutionReverb.h"

//==============================================================================
/**
*/
class SpiralLabDSPAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SpiralLabDSPAudioProcessor();
    ~SpiralLabDSPAudioProcessor() override;

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
   
    juce::AudioProcessorValueTreeState apvts;
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    SpiralConvolutionReverb rvb;
    std::vector<float> testIR;
    
    /*
    SpiralRandComb randComb;
    bool lastTriggerState = false;
     */
    
    SpiralRandWipe randWipe;
    bool lastTriggerState = false;
    
    juce::AudioBuffer<float> sourceBBuffer;
    int sourceBReadPos = 0;
    
    void pushInputToScope  (const float* monoData, int numSamples);
    void pushOutputToScope (const float* monoData, int numSamples);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpiralLabDSPAudioProcessor)
};
