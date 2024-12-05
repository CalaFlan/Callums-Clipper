/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>

//==============================================================================
BasicClippingAudioProcessor::BasicClippingAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

BasicClippingAudioProcessor::~BasicClippingAudioProcessor()
{
}

//==============================================================================
const juce::String BasicClippingAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicClippingAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicClippingAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicClippingAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicClippingAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicClippingAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicClippingAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicClippingAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BasicClippingAudioProcessor::getProgramName (int index)
{
    return {};
}

void BasicClippingAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BasicClippingAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    rmsLevelLeft.reset(sampleRate, 0.5);
    rmsLevelRight.reset(sampleRate, 0.5);

    rmsLevelLeft.setTargetValue(-100.f);
    rmsLevelRight.setTargetValue(-100.f);
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void BasicClippingAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicClippingAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BasicClippingAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    {
        const auto value = Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
        if (value < rmsLevelLeft.getCurrentValue())
            rmsLevelLeft.setTargetValue(value);
        else
            rmsLevelLeft.setCurrentAndTargetValue(value);
    }
    {
        const auto value = Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
        if (value < rmsLevelRight.getCurrentValue())
            rmsLevelRight.setTargetValue(value);
        else
            rmsLevelRight.setCurrentAndTargetValue(value);
    }

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    // Selector
    float outputSample;
    float drySample;

    // Bypass
    if (!bypassEnabled)
    {
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            //threshold = .8;
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                // Input Gain
                channelData[sample] = channelData[sample] * inputGain;

                // Assign copies of signal for dry/wet mix
                outputSample = channelData[sample];
                drySample = channelData[sample];

                // switch between distortion algos
                switch (distortionType) 
                {
                case 0: // Hard Clip 
                    outputSample = HardClip(channelData[sample], threshold);
                    break;
                case 1: // Soft Clip
                    outputSample = SoftClip(channelData[sample], threshold);
                    break;
                case 2: // Jagged
                    outputSample = JaggedClip(channelData[sample], threshold);
                    break;
                case 3: // Rectifier
                    outputSample = Rectifier(channelData[sample], threshold);
                    break;
                case 4: // Gate Clip
                    outputSample = GateClip(channelData[sample], threshold);
                    break;
                }
                
                // Mix Stage
                outputSample = (outputSample * dryWetPercentage) + (drySample * (1 - dryWetPercentage));

                // Output Gain Stage
                channelData[sample] = outputSample * outputGain;
            }
        }
    }
    //Get RMS For level display
    rmsLevelLeft = Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    rmsLevelRight = Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
}

//==============================================================================
bool BasicClippingAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BasicClippingAudioProcessor::createEditor()
{
    return new BasicClippingAudioProcessorEditor (*this);
}

//==============================================================================
void BasicClippingAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BasicClippingAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

float BasicClippingAudioProcessor::getRmsValue(const int channel) const
{
    jassert(channel == 0 || channel == 1);
    if (channel == 0)
        return rmsLevelLeft.getCurrentValue();
    if (channel == 1)
        return rmsLevelRight.getCurrentValue();
    return 0.f;

}
//==============================================================================

// Distortion Algorithms

// hard Clipping
float BasicClippingAudioProcessor::HardClip(float inputSample, float threshold)
{
    float outputSample = inputSample;
    if (inputSample >= threshold)
    {
        outputSample = threshold;
    }
    else if (inputSample <= 0 - threshold && !Asymmetrystate)
    {
        outputSample = 0 - threshold;
    }
    return outputSample;
}
// Soft Clipping
float BasicClippingAudioProcessor::SoftClip(float inputSample, float threshold)
{
    float outputSample = inputSample;
    double th = 1.0 / 3.0; // threshold for symmetrical soft clipping

    if (abs(inputSample) < th) {
        if (inputSample > 0) 
        {
            outputSample = 2 * inputSample;
        }
        else if (inputSample < 0 && !Asymmetrystate)
        {
            outputSample = 2 * inputSample;
        }
    }
    else if (abs(inputSample) >= th) {
        if (inputSample > 0) {
            outputSample = (3 - pow(2 - inputSample * 3, 2)) / 3;
        }
        else if (!Asymmetrystate)
        {
            outputSample = -(3 - pow(2 - abs(inputSample) * 3, 2)) / 3;
        }
    }
    if (abs(inputSample) > 2 * th) {
        if (inputSample > 0) {
            outputSample = 1;
        }
        else if (!Asymmetrystate)
        {
            outputSample = -1;
        }
    }
    return outputSample;
}

float BasicClippingAudioProcessor::JaggedClip(float inputSample, float threshold)
{
    float outputSample = inputSample;
    if (inputSample >= threshold)
    {
        outputSample = threshold * 1.5 * (inputSample - (inputSample * inputSample * inputSample) / 3);
    }
    else if (inputSample <= 0 - threshold && !Asymmetrystate)
    {
        outputSample = threshold * 1.5 * (inputSample + (inputSample * inputSample * inputSample) / 3);
    }

    return outputSample;
}

float BasicClippingAudioProcessor::Rectifier(float inputSample, float threshold)
{
    float outputSample = inputSample;
    outputSample = (abs(inputSample) * 2) - (inputSample / 2);
    return outputSample;
}

float BasicClippingAudioProcessor::GateClip(float inputSample, float threshold)
{
    float outputSample = inputSample; 
    threshold = 1 - threshold; //invert threshold, so lower number = more distortion, to match other alogrithms
    if (inputSample > 0 && inputSample < threshold)
    {
        outputSample = 0;
    }
    else if (inputSample < 0 && inputSample > -threshold && !Asymmetrystate)
    {
        outputSample = 0;
    }
    return outputSample;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicClippingAudioProcessor();
}

