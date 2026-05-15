/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EffectsChainAudioProcessorEditor::EffectsChainAudioProcessorEditor (EffectsChainAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      iirCutoffAttachment(p.apvts, "iir_cutoff", iirCutoffSlider),
      iirBypassAttachment(p.apvts, "iir_bypass", iirBypassButton),
      reverbSizeAttachment(p.apvts, "reverb_size", reverbSizeSlider),
      reverbWetAttachment(p.apvts, "reverb_wet", reverbWetSlider),
      reverbBypassAttachment(p.apvts, "reverb_bypass", reverbBypassButton),
      distDriveAttachment(p.apvts, "dist_drive", distDriveSlider),
      distBypassAttachment(p.apvts, "dist_bypass", distBypassButton)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 400);
    
    auto addEffectOptions = [](juce::ComboBox& box) {
        box.addItem("IIR Filter", 1);
        box.addItem("Reverb", 2);
        box.addItem("Distortion", 3);
    };
    
    slot1Label.setText("Slot 1", juce::dontSendNotification);
    slot2Label.setText("Slot 2", juce::dontSendNotification);
    slot3Label.setText("Slot 3", juce::dontSendNotification);

    addEffectOptions(slot1Box); slot1Box.setSelectedId(1);
    addEffectOptions(slot2Box); slot2Box.setSelectedId(2);
    addEffectOptions(slot3Box); slot3Box.setSelectedId(3);

    slot1Box.onChange = [this] { updateEffectOrder(); };
    slot2Box.onChange = [this] { updateEffectOrder(); };
    slot3Box.onChange = [this] { updateEffectOrder(); };

    addAndMakeVisible(slot1Label); addAndMakeVisible(slot1Box);
    addAndMakeVisible(slot2Label); addAndMakeVisible(slot2Box);
    addAndMakeVisible(slot3Label); addAndMakeVisible(slot3Box);
    
    iirTitle.setText("IIR Filter", juce::dontSendNotification);
    iirTitle.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    iirCutoffLabel.setText("Cutoff", juce::dontSendNotification);
    iirCutoffSlider.setSliderStyle(juce::Slider::Rotary);
    iirCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);

    addAndMakeVisible(iirTitle);
    addAndMakeVisible(iirCutoffLabel);
    addAndMakeVisible(iirCutoffSlider);
    addAndMakeVisible(iirBypassButton);
    
    reverbTitle.setText("Reverb", juce::dontSendNotification);
    reverbTitle.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    reverbSizeLabel.setText("Room Size", juce::dontSendNotification);
    reverbWetLabel.setText("Wet", juce::dontSendNotification);
    reverbSizeSlider.setSliderStyle(juce::Slider::Rotary);
    reverbWetSlider.setSliderStyle(juce::Slider::Rotary);
    reverbSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    reverbWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);

    addAndMakeVisible(reverbTitle);
    addAndMakeVisible(reverbSizeLabel);
    addAndMakeVisible(reverbWetLabel);
    addAndMakeVisible(reverbSizeSlider);
    addAndMakeVisible(reverbWetSlider);
    addAndMakeVisible(reverbBypassButton);
    
    distTitle.setText("Distortion", juce::dontSendNotification);
    distTitle.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    distDriveLabel.setText("Drive", juce::dontSendNotification);
    distDriveSlider.setSliderStyle(juce::Slider::Rotary);
    distDriveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);

    addAndMakeVisible(distTitle);
    addAndMakeVisible(distDriveLabel);
    addAndMakeVisible(distDriveSlider);
    addAndMakeVisible(distBypassButton);
}

EffectsChainAudioProcessorEditor::~EffectsChainAudioProcessorEditor()
{
}

//==============================================================================
void EffectsChainAudioProcessorEditor::updateEffectOrder()
{
    // ComboBox IDs are 1=IIR, 2=Reverb, 3=Distortion
    // effectOrder uses 0=IIR, 1=Reverb, 2=Distortion so subtract 1
    audioProcessor.setEffectOrder(0, slot1Box.getSelectedId() - 1);
    audioProcessor.setEffectOrder(1, slot2Box.getSelectedId() - 1);
    audioProcessor.setEffectOrder(2, slot3Box.getSelectedId() - 1);
}

void EffectsChainAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    
    g.setColour(juce::Colours::grey);
    g.drawLine(210, 80, 210, 390, 1.0f);
    g.drawLine(410, 80, 410, 390, 1.0f);
}

void EffectsChainAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    slot1Label.setBounds(10,  10, 60, 20);
    slot1Box.setBounds  (70,  10, 130, 25);
    slot2Label.setBounds(210, 10, 60, 20);
    slot2Box.setBounds  (270, 10, 130, 25);
    slot3Label.setBounds(410, 10, 60, 20);
    slot3Box.setBounds  (470, 10, 130, 25);
    
    iirTitle.setBounds       (10,  50, 190, 25);
    iirBypassButton.setBounds(10,  80, 100, 25);
    iirCutoffLabel.setBounds (10, 115, 190, 20);
    iirCutoffSlider.setBounds(10, 135, 190, 180);
    
    reverbTitle.setBounds       (215,  50, 190, 25);
    reverbBypassButton.setBounds(215,  80, 100, 25);
    reverbSizeLabel.setBounds   (215, 115, 90,  20);
    reverbSizeSlider.setBounds  (215, 135, 90,  150);
    reverbWetLabel.setBounds    (315, 115, 90,  20);
    reverbWetSlider.setBounds   (315, 135, 90,  150);
    
    distTitle.setBounds       (415,  50, 190, 25);
    distBypassButton.setBounds(415,  80, 100, 25);
    distDriveLabel.setBounds  (415, 115, 190, 20);
    distDriveSlider.setBounds (415, 135, 190, 180);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EffectsChainAudioProcessor();
}
