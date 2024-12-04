/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "VerticalMeter.h"
#include <iostream>
#include <string>
#include <map>

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
    void timerCallback() override;     // Timer for RMS Display



private:
    // Functions
    void sliderValueChanged(juce::Slider* slider) override; // [3]
    void incrementDistortionType();
    void decrementDistortionType();
    std::string FindDistortionNameByIndex(int index);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BasicClippingAudioProcessor& audioProcessor;
    juce::Slider InputVolume;
    juce::Slider OutputVolume;
    juce::Slider ThresholdSlider;
    juce::Slider MixSlider;

    juce::ToggleButton BypassToggle; 
    juce::Label DistortionType;

    // Toggle Asymetry
    juce::ToggleButton AsymetryToggle;

    // Selection buttons
    juce::TextButton TypeIncrementButton;
    juce::TextButton TypeDecrementButton;

    // Metering

    Gui::VerticalMeter verticalMeterL, verticalMeterR;

    //Create a map to allow us to find the distortion types name using the int value DistortionType
    std::map<int, std::string> DistortionNameMap
    {
        { 0, "Hard Clip"}, 
        { 1, "Soft Clip" },
        { 2, "Jagged Clip" },
        { 3, "Rectifier" }
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicClippingAudioProcessorEditor)
};
