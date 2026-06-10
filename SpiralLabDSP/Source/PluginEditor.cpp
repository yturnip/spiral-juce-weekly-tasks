/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpiralLabDSPAudioProcessorEditor::SpiralLabDSPAudioProcessorEditor (SpiralLabDSPAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    // Generic parameter editor
    paramEditor.reset (new juce::GenericAudioProcessorEditor (p));
    addAndMakeVisible (paramEditor.get());

    inputScope.setBufferSize (256);
    inputScope.setSamplesPerBlock (64);  
    inputScope.setColours (juce::Colours::darkgrey, juce::Colours::lightgreen);
    
    outputScope.setBufferSize (256);
    outputScope.setSamplesPerBlock (64);
    outputScope.setColours (juce::Colours::darkgrey, juce::Colours::orange);

    addAndMakeVisible (inputScope);
    addAndMakeVisible (outputScope);

    setSize (700, 700);

    startTimerHz (30); // repaint regularly
}

SpiralLabDSPAudioProcessorEditor::~SpiralLabDSPAudioProcessorEditor()
{
}

//==============================================================================
void SpiralLabDSPAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
}

void SpiralLabDSPAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds();
    auto paramArea = area.removeFromBottom (area.getHeight() / 4);
    paramEditor->setBounds (paramArea);

    auto topArea = area;
    auto topLeft  = topArea.removeFromLeft (topArea.getWidth() / 2);
    inputScope.setBounds (topLeft);
    outputScope.setBounds (topArea);
}

void SpiralLabDSPAudioProcessorEditor::timerCallback()
{
    // AudioVisualiserComponent handles drawing internally; timer just triggers repaints
    repaint();
}

void SpiralLabDSPAudioProcessorEditor::pushInputSamples (const float* samples, int numSamples)
{
    const float* channels[] = { samples };
    inputScope.pushBuffer (channels, 1, numSamples);
}

void SpiralLabDSPAudioProcessorEditor::pushOutputSamples (const float* samples, int numSamples)
{
    const float* channels[] = { samples };
    outputScope.pushBuffer (channels, 1, numSamples);
}
