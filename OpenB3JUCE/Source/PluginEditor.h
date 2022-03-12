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

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

class OpenB3AudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::MidiKeyboardState::Listener
{
public:
    OpenB3AudioProcessorEditor (OpenB3AudioProcessor&);
    ~OpenB3AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OpenB3AudioProcessor& audioProcessor;

    juce::MidiKeyboardComponent midiKeyboardUpperManual;
    juce::MidiKeyboardComponent midiKeyboardLowerManual;
    juce::MidiKeyboardComponent midiKeyboardPedalBoard;

    juce::MidiKeyboardState keyboardState;
    void handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

    int n_white_keys_manual;
    int n_white_keys_pedalboard;
    int n_manuals;
    void init_keyboards();

    juce::TextButton vibrato_lower, vibrato_upper;
    juce::Label vibrato_chorus_label, C1_label, V2_label, C2_label, V3_label, C3_label, V1_label;
    juce::Slider vibrato_chorus;
    juce::TextButton perc_on_off, perc_soft_norm, perc_fast_slow, perc_2nd_3rd;
    juce::TextButton overdrive;
    juce::Label gain_label, rotary_label, reverb_label, volume_label;
    juce::Label rotary_slow_label, rotary_stop_label, rotary_fast_label;
    juce::Slider gain, rotary, reverb, volume;
    void init_control_board();

    juce::Label drawbar_upper_label, drawbar_lower_label, drawbar_pedalboard_label;
    juce::Slider drawbar_upper[9], drawbar_lower[9], drawbar_pedalboard[2];
    void init_drawbars();

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
            vibrato_upper_attachment, vibrato_lower_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
            vibrato_chorus_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
            perc_on_off_attachment, perc_soft_norm_attachment, perc_fast_slow_attachment, perc_2nd_3rd_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
            overdrive_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
            gain_attachment, rotary_attachment, reverb_attachment, volume_attachment;
    void init_control_board_attachments();

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
            drawbar_upper_attachment[9], drawbar_lower_attachment[9], drawbar_pedalboard_attachment[2];
    void init_drawbars_attachments();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenB3AudioProcessorEditor)
};
