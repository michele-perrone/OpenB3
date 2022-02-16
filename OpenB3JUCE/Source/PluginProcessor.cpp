/*
OpenPiano: an open source piano engine based on physical modeling
Copyright (C) 2021-2022 Michele Perrone
Github: https://github.com/michele-perrone/OpenPiano
Author e-mail: perrone(dot)michele(at)outlook(dot)com
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OpenB3AudioProcessor::OpenB3AudioProcessor()
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

OpenB3AudioProcessor::~OpenB3AudioProcessor()
{
}

//==============================================================================
const juce::String OpenB3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OpenB3AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OpenB3AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OpenB3AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OpenB3AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OpenB3AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OpenB3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void OpenB3AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OpenB3AudioProcessor::getProgramName (int index)
{
    return {};
}

void OpenB3AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OpenB3AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    beatrix = new Beatrix(sampleRate);
}

void OpenB3AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    delete beatrix;
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OpenB3AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void OpenB3AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
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
    keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);

    for (const auto metadata : midiMessages)
    {
        juce::MidiMessage message = metadata.getMessage();

        switch (message.getChannel())
        {
        case 1: // Upper Manual
            if (message.isNoteOn())
                beatrix->note_on(message.getNoteNumber()-36);
            else if(message.isNoteOff())
                beatrix->note_off(message.getNoteNumber()-36);
            break;
        case 2: // Lower Manual
            if (message.isNoteOn())
                beatrix->note_on(message.getNoteNumber()+28);
            else if(message.isNoteOff())
                beatrix->note_off(message.getNoteNumber()+28);
            break;
        case 3: // Pedal Board
            if (message.isNoteOn())
                beatrix->note_on(message.getNoteNumber()+92);
            else if(message.isNoteOff())
                beatrix->note_off(message.getNoteNumber()+92);
            break;
        default:
            break;
        }
    }

    size_t samplesPerBlock = (size_t)buffer.getNumSamples();
    float* outputChannelData_L = buffer.getWritePointer(0);
    float* outputChannelData_R = buffer.getWritePointer(1);
    beatrix->get_next_block(outputChannelData_L, outputChannelData_R, 1, samplesPerBlock);
}

//==============================================================================
bool OpenB3AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OpenB3AudioProcessor::createEditor()
{
    return new OpenB3AudioProcessorEditor (*this);
}

//==============================================================================
void OpenB3AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void OpenB3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OpenB3AudioProcessor();
}
