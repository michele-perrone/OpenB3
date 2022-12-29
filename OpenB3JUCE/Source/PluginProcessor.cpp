/*
OpenB3: an open source sound synthesis engine and JUCE application/plugin that simulates
the magnificent sound of the Hammond B3 organ and Leslie rotating speaker
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
                       ), apvts(*this, nullptr, "Main parameters", createParameters())
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
    // In OpenB3, we only support stereo output.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

    // Process the MIDI messages coming from the keyboards and append them to the midi buffer
    keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);

    for (const auto metadata : midiMessages)
    {
        juce::MidiMessage message = metadata.getMessage();

        size_t raw_message_size = (size_t)message.getRawDataSize();

        // All messages need to be 3 bytes except program-changes (2 bytes)
        if(raw_message_size == 2 || raw_message_size == 3)
        {
            // Forward the current midi message to Beatrix
            beatrix->process_midi_message(message.getRawData(), raw_message_size);
        }
    }

    // Compute the next audio block
    size_t samplesPerBlock = (size_t)buffer.getNumSamples();
    float* outputChannelData_L = buffer.getWritePointer(0);
    float* outputChannelData_R = buffer.getWritePointer(1);
    beatrix->get_next_block(outputChannelData_L, outputChannelData_R, samplesPerBlock);
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
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OpenB3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if(xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OpenB3AudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout OpenB3AudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterBool>("VIBRATO_UPPER", "Vibrato Upper", false));

    parameters.push_back(std::make_unique<juce::AudioParameterBool>("VIBRATO_LOWER", "Vibrato Lower", false));

    parameters.push_back(std::make_unique<juce::AudioParameterInt>("VIBRATO_CHORUS", "Vibrato&Chorus", 0, 5, 0));

    parameters.push_back(std::make_unique<juce::AudioParameterBool>("PERC_ON_OFF", "Percussion On/Off", false));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("PERC_SOFT_NORM", "Percussion Soft/Norm.", false));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("PERC_FAST_SLOW", "Percussion Fast/Slow", false));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("PERC_2ND_3RD", "Percussion 2nd/3rd", false));

    parameters.push_back(std::make_unique<juce::AudioParameterBool>("OVERDRIVE", "Overdrive", false));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", 0.0f, 1.0f, 0.1f));

    parameters.push_back(std::make_unique<juce::AudioParameterInt>("ROTARY", "Rotary", 0, 2, 0));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("REVERB", "Reverb", 0.0f, 1.0f, 0.2f));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("VOLUME", "Volume", 0.0f, 1.0f, 0.75f));

    char parameterID[24];
    char parameterName[24];

    for(int i = 0; i < 9; i++)
    {
        sprintf(parameterID, "DRAWBAR_UPPER_%i", i);
        sprintf(parameterName, "Drawbar Upper %i", i);
        parameters.push_back(std::make_unique<juce::AudioParameterInt>
                             (parameterID, parameterName, 0, 8, defaultPresetUpperManual[i]));
    }

    for(int i = 0; i < 9; i++)
    {
        sprintf(parameterID, "DRAWBAR_LOWER_%i", i);
        sprintf(parameterName, "Drawbar Lower %i", i);
        parameters.push_back(std::make_unique<juce::AudioParameterInt>
                             (parameterID, parameterName, 0, 8, defaultPresetLowerManual[i]));
    }

    for(int i = 0; i < 2; i++)
    {
        sprintf(parameterID, "DRAWBAR_PEDALBOARD_%i", i);
        sprintf(parameterName, "Drawbar Pedalboard %i", i);
        parameters.push_back(std::make_unique<juce::AudioParameterInt>
                             (parameterID, parameterName, 0, 8, defaultPresetPedalBoard[i]));
    }

    return  { parameters.begin(), parameters.end() };
}

void OpenB3AudioProcessor::parameterChanged (const String &parameterID, float newValue)
{
    if(parameterID == "VIBRATO_UPPER")
        beatrix->set_vibrato_upper((bool)newValue);

    else if(parameterID == "VIBRATO_LOWER")
        beatrix->set_vibrato_lower((bool)newValue);

    else if(parameterID == "VIBRATO_CHORUS")
        beatrix->set_vibrato((int)newValue);

    else if(parameterID == "PERC_ON_OFF")
        beatrix->set_percussion_enabled((bool)newValue);
    else if(parameterID == "PERC_SOFT_NORM")
        beatrix->set_percussion_volume((bool)newValue);
    else if(parameterID == "PERC_FAST_SLOW")
        beatrix->set_percussion_fast((bool)newValue);
    else if(parameterID == "PERC_2ND_3RD")
        beatrix->set_percussion_first((bool)newValue);

    else if(parameterID == "OVERDRIVE")
        beatrix->set_preamp_clean(!(bool)newValue);
    else if(parameterID == "GAIN")
        beatrix->set_input_gain((float)newValue);

    else if(parameterID == "ROTARY")
        beatrix->set_rotary_speed((int)newValue);
    else if(parameterID == "REVERB")
        beatrix->set_reverb_dry_wet((float)newValue);

    else if(parameterID == "VOLUME")
        beatrix->set_swell((float)newValue);

    else if(parameterID.startsWith("DRAWBAR_UPPER"))
    {
        uint32_t upper_manual_drawbars[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        char _parameterID[24];
        for(int i = 0; i < 9; i++)
        {
            if(parameterID.endsWithChar(i+'0'))
            {
                upper_manual_drawbars[i] = (uint32_t)newValue;
            }
            else
            {
                sprintf(_parameterID, "DRAWBAR_UPPER_%i", i);
                upper_manual_drawbars[i] = apvts.getRawParameterValue(_parameterID)->load();
            }
        }
        beatrix->set_drawbars(UPPER_MANUAL, upper_manual_drawbars);
    }

    else if(parameterID.startsWith("DRAWBAR_LOWER"))
    {
        uint32_t lower_manual_drawbars[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        char _parameterID[24];
        for(int i = 0; i < 9; i++)
        {
            if(parameterID.endsWithChar(i+'0'))
            {
                lower_manual_drawbars[i] = (uint32_t)newValue;
            }
            else
            {
                sprintf(_parameterID, "DRAWBAR_LOWER_%i", i);
                lower_manual_drawbars[i] = apvts.getRawParameterValue(_parameterID)->load();
            }
        }
        beatrix->set_drawbars(LOWER_MANUAL, lower_manual_drawbars);
    }

    else if(parameterID.startsWith("DRAWBAR_LOWER"))
    {
        uint32_t pedalboard_drawbars[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        if(parameterID.endsWithChar('0'))
        {
            pedalboard_drawbars[0] = (uint32_t)newValue;
            pedalboard_drawbars[1] = apvts.getRawParameterValue("DRAWBAR_LOWER_1")->load();
        }
        else if(parameterID.endsWithChar('1'))
        {
            pedalboard_drawbars[1] = apvts.getRawParameterValue("DRAWBAR_LOWER_0")->load();
            pedalboard_drawbars[1] = (uint32_t)newValue;
        }
        beatrix->set_drawbars(PEDAL_BOARD, pedalboard_drawbars);
    }
}
