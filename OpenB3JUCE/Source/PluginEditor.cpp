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
    setResizable(true, true);

    int screen_width = Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea.getWidth();
    int screen_height = Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea.getHeight();
    setSize (screen_height*3/4, screen_height/4);
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
    juce::Grid grid;

    using Track = juce::Grid::TrackInfo;
    grid.templateRows = { Track(1_fr), Track(1_fr), Track(1_fr) };
    grid.templateColumns = { Track(1_fr), Track(1_fr), Track(1_fr) };
    grid.rowGap = juce::Grid::Px(3);

    midiKeyboardUpperManual.setKeyWidth(getLocalBounds().getWidth()/(float)n_white_keys_manual);
    midiKeyboardLowerManual.setKeyWidth(getLocalBounds().getWidth()/(float)n_white_keys_manual);
    midiKeyboardPedalBoard.setKeyWidth(getLocalBounds().getWidth()/(float)n_white_keys_manual);
    grid.items =
    {
        GridItem(midiKeyboardUpperManual).withArea(1, 1, 2, 4),
        GridItem(midiKeyboardLowerManual).withArea(2, 1, 3, 4),
        GridItem(midiKeyboardPedalBoard).withWidth(midiKeyboardPedalBoard.getKeyWidth()*n_white_keys_pedalboard+0.1)
                                        .withJustifySelf(juce::GridItem::JustifySelf::center)
                                        .withArea(3, 2, 4, 3)
    };
    grid.performLayout(getLocalBounds());
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
    n_white_keys_manual = 36; // 5 octaves
    n_white_keys_pedalboard = 19;
    n_manuals = 3;

    // Upper manual
    midiKeyboardUpperManual.setName ("Upper Manual");
    midiKeyboardUpperManual.setTitle("Upper Manual");
    midiKeyboardUpperManual.setAvailableRange(36, 96);
    midiKeyboardUpperManual.setOctaveForMiddleC(4);
    midiKeyboardUpperManual.setMidiChannel(1); // From 1 to 16
    midiKeyboardUpperManual.setMidiChannelsToDisplay(0x1); // Binary mask, one bit for each channel
    addAndMakeVisible (midiKeyboardUpperManual);

    // Lower manual
    midiKeyboardLowerManual.setName ("Lower Manual");
    midiKeyboardLowerManual.setTitle ("Lower Manual");
    midiKeyboardLowerManual.setAvailableRange(36, 96);
    midiKeyboardLowerManual.setOctaveForMiddleC(4);
    midiKeyboardLowerManual.setMidiChannel(2);
    midiKeyboardLowerManual.setMidiChannelsToDisplay(0x2);
    addAndMakeVisible (midiKeyboardLowerManual);

    // Pedal board
    midiKeyboardPedalBoard.setName ("Pedal Board");
    midiKeyboardPedalBoard.setTitle ("Pedal Board");
    midiKeyboardPedalBoard.setAvailableRange(36, 67);
    midiKeyboardPedalBoard.setOctaveForMiddleC(4);
    midiKeyboardPedalBoard.setMidiChannel(3);
    midiKeyboardPedalBoard.setMidiChannelsToDisplay(0x4);
    addAndMakeVisible (midiKeyboardPedalBoard);

    keyboardState.addListener(this);
}
