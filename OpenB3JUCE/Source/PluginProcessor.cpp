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

    // Process the MIDI messages.
    // Right now, only noteOn/noteOff messages are dealth with.
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

    // Get the control board parameters/settings and forward them to Beatrix
    beatrix->set_vibrato_upper(apvts.getRawParameterValue("VIBRATO UPPER")->load());
    beatrix->set_vibrato_lower(apvts.getRawParameterValue("VIBRATO LOWER")->load());
    beatrix->set_vibrato(apvts.getRawParameterValue("VIBRATO_CHORUS")->load());

    beatrix->set_percussion_enabled(apvts.getRawParameterValue("PERC_ON_OFF")->load());
    beatrix->set_percussion_volume(apvts.getRawParameterValue("PERC_SOFT_NORM")->load());
    beatrix->set_percussion_fast(apvts.getRawParameterValue("PERC_FAST_SLOW")->load());
    beatrix->set_percussion_first(apvts.getRawParameterValue("PERC_2ND_3RD")->load());

    beatrix->set_preamp_clean(!apvts.getRawParameterValue("OVERDRIVE")->load());
    beatrix->set_input_gain(apvts.getRawParameterValue("GAIN")->load());

    beatrix->set_rotary_speed(apvts.getRawParameterValue("ROTARY")->load());
    beatrix->set_reverb_dry_wet(apvts.getRawParameterValue("REVERB")->load());

    beatrix->set_swell(apvts.getRawParameterValue("VOLUME")->load());

    // Get the drawbar settings and forward them to Beatrix
    uint32_t upper_manual_drawbars[9];
    char parameterID[24];
    for(int i = 0; i < 9; i++)
    {
        sprintf(parameterID, "DRAWBAR UPPER %i", i);
        upper_manual_drawbars[i] = apvts.getRawParameterValue(parameterID)->load();
    }
    beatrix->set_drawbars(UPPER_MANUAL, upper_manual_drawbars);

    uint32_t lower_manual_drawbars[9];
    for(int i = 0; i < 9; i++)
    {
        sprintf(parameterID, "DRAWBAR LOWER %i", i);
        lower_manual_drawbars[i] = apvts.getRawParameterValue(parameterID)->load();
    }
    beatrix->set_drawbars(LOWER_MANUAL, lower_manual_drawbars);

    uint32_t pedalboard_drawbars[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    for(int i = 0; i < 2; i++)
    {
        sprintf(parameterID, "DRAWBAR PEDALBOARD %i", i);
        pedalboard_drawbars[i] = apvts.getRawParameterValue(parameterID)->load();
    }
    beatrix->set_drawbars(PEDAL_BOARD, pedalboard_drawbars);

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

juce::AudioProcessorValueTreeState::ParameterLayout OpenB3AudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterBool>("VIBRATO UPPER", "Vibrato Upper", false));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("VIBRATO LOWER", "Vibrato Lower", false));

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
        sprintf(parameterID, "DRAWBAR UPPER %i", i);
        sprintf(parameterName, "Drawbar Upper %i", i);
        parameters.push_back(std::make_unique<juce::AudioParameterInt>
                             (parameterID, parameterName, 0, 8, defaultPresetUpperManual[i]));
    }

    for(int i = 0; i < 9; i++)
    {
        sprintf(parameterID, "DRAWBAR LOWER %i", i);
        sprintf(parameterName, "Drawbar Lower %i", i);
        parameters.push_back(std::make_unique<juce::AudioParameterInt>
                             (parameterID, parameterName, 0, 8, defaultPresetLowerManual[i]));
    }

    for(int i = 0; i < 2; i++)
    {
        sprintf(parameterID, "DRAWBAR PEDALBOARD %i", i);
        sprintf(parameterName, "Drawbar Pedalboard %i", i);
        parameters.push_back(std::make_unique<juce::AudioParameterInt>
                             (parameterID, parameterName, 0, 8, defaultPresetPedalBoard[i]));
    }

    return  { parameters.begin(), parameters.end() };
}




