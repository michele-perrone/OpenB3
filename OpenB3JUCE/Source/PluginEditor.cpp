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
OpenB3AudioProcessorEditor::OpenB3AudioProcessorEditor (OpenB3AudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      midiKeyboardUpperManual(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
      midiKeyboardLowerManual(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
      midiKeyboardPedalBoard(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    init_keyboards();

    setSize (keyboards_width, keyboards_height);
}

OpenB3AudioProcessorEditor::~OpenB3AudioProcessorEditor()
{
    keyboardState.removeListener(this);
}

//==============================================================================
void OpenB3AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(Colours::black);
}

void OpenB3AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    midiKeyboardUpperManual.setBounds(0, 0,               getWidth(), getHeight()/3);
    midiKeyboardLowerManual.setBounds(0, getHeight()/3,   getWidth(), getHeight()/3);
    midiKeyboardPedalBoard.setBounds (white_key_width*(n_white_keys_manual-n_white_keys_pedalboard)/2, // To the middle
                                      2*getHeight()/3,
                                      n_white_keys_pedalboard*white_key_width,
                                      getHeight()/3);
}

void OpenB3AudioProcessorEditor::handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    keyboardState.noteOn(midiChannel, midiNoteNumber, velocity);
}

void OpenB3AudioProcessorEditor::handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    keyboardState.noteOn(midiChannel, midiNoteNumber, velocity);
}

void OpenB3AudioProcessorEditor::init_keyboards()
{
    white_key_width = 24;
    n_white_keys_manual = 36; // 5 octaves
    n_white_keys_pedalboard = 19;
    n_manuals = 3;
    manual_height = 128;
    keyboards_width = white_key_width*n_white_keys_manual;
    keyboards_height = manual_height*n_manuals;

    // Upper manual
    midiKeyboardUpperManual.setName ("Upper Manual");
    midiKeyboardUpperManual.setTitle("Upper Manual");
    midiKeyboardUpperManual.setAvailableRange(36, 96);
    midiKeyboardUpperManual.setKeyWidth(white_key_width);
    midiKeyboardUpperManual.setOctaveForMiddleC(4);
    midiKeyboardUpperManual.setMidiChannel(1); // From 1 to 16
    midiKeyboardUpperManual.setMidiChannelsToDisplay(0x1); // Binary mask, one bit for each channel
    addAndMakeVisible (midiKeyboardUpperManual);

    // Lower manual
    midiKeyboardLowerManual.setName ("Lower Manual");
    midiKeyboardLowerManual.setTitle ("Lower Manual");
    midiKeyboardLowerManual.setAvailableRange(36, 96);
    midiKeyboardLowerManual.setKeyWidth(white_key_width);
    midiKeyboardLowerManual.setOctaveForMiddleC(4);
    midiKeyboardLowerManual.setMidiChannel(2);
    midiKeyboardLowerManual.setMidiChannelsToDisplay(0x2);
    addAndMakeVisible (midiKeyboardLowerManual);

    // Pedal board
    midiKeyboardPedalBoard.setName ("Pedal Board");
    midiKeyboardPedalBoard.setTitle ("Pedal Board");
    midiKeyboardPedalBoard.setAvailableRange(36, 67);
    midiKeyboardPedalBoard.setKeyWidth(white_key_width);
    midiKeyboardPedalBoard.setOctaveForMiddleC(4);
    midiKeyboardPedalBoard.setMidiChannel(3);
    midiKeyboardPedalBoard.setMidiChannelsToDisplay(0x4);
    addAndMakeVisible (midiKeyboardPedalBoard);

    keyboardState.addListener(this);
}
