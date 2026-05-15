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
class EffectsChainAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    EffectsChainAudioProcessorEditor (EffectsChainAudioProcessor&);
    ~EffectsChainAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EffectsChainAudioProcessor& audioProcessor;
    
    juce::Label slot1Label, slot2Label, slot3Label;
    juce::ComboBox slot1Box, slot2Box, slot3Box;
    
    juce::Label iirTitle, iirCutoffLabel;
    juce::Slider iirCutoffSlider;
    juce::ToggleButton iirBypassButton { "Bypass" };
    juce::AudioProcessorValueTreeState::SliderAttachment iirCutoffAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment iirBypassAttachment;
    
    juce::Label reverbTitle, reverbSizeLabel, reverbWetLabel;
    juce::Slider reverbSizeSlider, reverbWetSlider;
    juce::ToggleButton reverbBypassButton { "Bypass" };
    juce::AudioProcessorValueTreeState::SliderAttachment reverbSizeAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment reverbWetAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment reverbBypassAttachment;
    
    juce::Label distTitle, distDriveLabel;
    juce::Slider distDriveSlider;
    juce::ToggleButton distBypassButton { "Bypass" };
    juce::AudioProcessorValueTreeState::SliderAttachment distDriveAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment distBypassAttachment;

    void updateEffectOrder();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectsChainAudioProcessorEditor)
};
