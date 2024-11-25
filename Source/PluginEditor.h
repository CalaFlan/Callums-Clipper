/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "VerticalMeter.h"

//==============================================================================
/**
*/
class BasicClippingAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener, public Timer
{
public:
    BasicClippingAudioProcessorEditor (BasicClippingAudioProcessor&);
    ~BasicClippingAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    // Timer for RMS Display

private:
    void sliderValueChanged(juce::Slider* slider) override; // [3]

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BasicClippingAudioProcessor& audioProcessor;
    juce::Slider InputVolume;
    juce::Slider OutputVolume;
    juce::Slider ThresholdSlider;
    juce::ToggleButton BypassToggle; 

    //hello

    // Metering
    Gui::VerticalMeter verticalMeterL, verticalMeterR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicClippingAudioProcessorEditor)
};
