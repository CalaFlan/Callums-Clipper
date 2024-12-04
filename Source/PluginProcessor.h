/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class BasicClippingAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    BasicClippingAudioProcessor();
    ~BasicClippingAudioProcessor() override;

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

    juce::SmoothedValue<float> volume{ 0.0f };

    float HardClip(float inputSample, float threshold);
    float SoftClip(float inputSample, float threshold);
    float JaggedClip(float inputSample, float threshold);
    float Rectifier(float inputSample, float threshold);

    //==============================================================================
    float inputGain;
    float outputGain;
    float threshold;
    float dryWetPercentage; 
    int distortionType = 0;
    int distortionTypeMaxValue=3;
    bool bypassEnabled;

    float getRmsValue(const int channel) const;

private:
    //==============================================================================
    LinearSmoothedValue <float> rmsLevelLeft, rmsLevelRight;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicClippingAudioProcessor)

};
