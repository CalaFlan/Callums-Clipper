/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
BasicClippingAudioProcessorEditor::BasicClippingAudioProcessorEditor (BasicClippingAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 300);


    // Input Volume
    InputVolume.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag); //LinearVertical
    InputVolume.setRange (0.0, 10.0, 0.02);
    InputVolume.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 150, 20);
    InputVolume.setPopupDisplayEnabled(true, false, this);
    InputVolume.setTextValueSuffix(" Drive");
    InputVolume.setValue(1.0);
    InputVolume.setBounds(10, 10, 150, 150);

    addAndMakeVisible(&InputVolume);
    InputVolume.addListener(this);

    // Thrshold
    ThresholdSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    ThresholdSlider.setRange(0.0, 1, 0.02);;
    ThresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 150, 20);
    ThresholdSlider.setPopupDisplayEnabled(true, false, this);
    ThresholdSlider.setTextValueSuffix(" Threshold");
    //ThresholdSlider.setSkewFactorFromMidPoint(.5);
    ThresholdSlider.setValue(.7);
    ThresholdSlider.setBounds(110, 110,150,150);

    addAndMakeVisible(&ThresholdSlider);
    ThresholdSlider.addListener(this);

    // Output Volume
    OutputVolume.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    OutputVolume.setRange(0.0, 2.0, .02);;
    OutputVolume.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 150, 20);
    OutputVolume.setPopupDisplayEnabled(true, false, this);
    OutputVolume.setTextValueSuffix(" Output Volume");
    OutputVolume.setValue(1.0);
    OutputVolume.setBounds(210, 10, 150, 150);

    addAndMakeVisible(&OutputVolume);
    OutputVolume.addListener(this);

    // Metering
    startTimerHz(24);
    addAndMakeVisible(verticalMeterL);
    addAndMakeVisible(verticalMeterR);
}

BasicClippingAudioProcessorEditor::~BasicClippingAudioProcessorEditor()
{
}

//==============================================================================
void BasicClippingAudioProcessorEditor::paint (juce::Graphics& g)
{
    // fill the whole window white
    g.fillAll(juce::Colours::black);

    // set the current drawing colour to black
    g.setColour(juce::Colours::black);

    // set the font size and draw text to the screen
    g.setFont(15.0f);
    // g.drawFittedText("Input Volume", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void BasicClippingAudioProcessorEditor::resized()
{
    //InputVolume.setBounds(40, 30, 20, getHeight() - 60);
    //ThresholdSlider.setBounds(140, 30, 20, getHeight() - 60);
    //OutputVolume.setBounds(240, 30, 20, getHeight() - 60);

    // RMS Output Graph 25 Gap
    verticalMeterL.setBounds(480, 30, 20, getHeight() - 60);
    verticalMeterR.setBounds(505, 30 , 20, getHeight() - 60);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void BasicClippingAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.inputGain = InputVolume.getValue();
    audioProcessor.threshold = ThresholdSlider.getValue();
    audioProcessor.outputGain = OutputVolume.getValue();
}

void BasicClippingAudioProcessorEditor::timerCallback()
{
    verticalMeterL.setlevel(audioProcessor.getRmsValue(0));
    verticalMeterR.setlevel(audioProcessor.getRmsValue(1));

    verticalMeterL.repaint();
    verticalMeterR.repaint();
}