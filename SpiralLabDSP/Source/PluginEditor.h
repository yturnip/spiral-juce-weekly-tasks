/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SpiralLabDSPAudioProcessorEditor  : public juce::AudioProcessorEditor,
private juce::Timer
{
public:
    SpiralLabDSPAudioProcessorEditor (SpiralLabDSPAudioProcessor&);
    ~SpiralLabDSPAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void pushInputSamples  (const float* samples, int numSamples);
    void pushOutputSamples (const float* samples, int numSamples);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    void timerCallback() override;
    
    SpiralLabDSPAudioProcessor& audioProcessor;
    
    std::unique_ptr<juce::GenericAudioProcessorEditor> paramEditor;
    
    juce::AudioVisualiserComponent inputScope  { 1 }; 
    juce::AudioVisualiserComponent outputScope { 1 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpiralLabDSPAudioProcessorEditor)
};
