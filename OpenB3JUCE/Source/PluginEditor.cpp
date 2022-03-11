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
    init_control_board();
    init_drawbars();
    create_attachments();
    init_keyboards();
    setResizable(true, true);

    int screen_width = Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea.getWidth();
    int screen_height = Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea.getHeight();
    setSize (screen_width*2/3, screen_height*3/4);
}

OpenB3AudioProcessorEditor::~OpenB3AudioProcessorEditor()
{
    keyboardState.removeListener(this);
}

//==============================================================================
void OpenB3AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromHSV (0.0f, 0.5f, 0.12f, 1.0f));
}

void OpenB3AudioProcessorEditor::resized()
{
    /*********************************************************************************
     * The GUI layout is horizontal and organized inside a grid structure, where:    *
     * 1. Each row can have a different height                                       *
     * 2. Each row can have a different number of columns, and                       *
     *    each column belonging to a row can have a different width                  *
     * 3. All the row, column and element sizes are expressed as fractions.          *
     *    Hardcoding pixel values is avoided at all costs.                           *
     *********************************************************************************/


    Rectangle<int> area(getLocalBounds());
    int row_1_height = area.getHeight()*0.24;
    int row_2_height = area.getHeight()*0.24;
    int row_3_height = area.getHeight()*0.24;
    int row_4_height = area.getHeight()*0.093333333;
    int row_5_height = area.getHeight()*0.093333333;
    int row_6_height = area.getHeight()*0.093333333;
    int row_width = area.getWidth();
    int margin = sqrt(area.getHeight()*area.getWidth())/150;
    int double_margin = margin*2;
    int triple_margin = margin*3;


    /***************************** Row 1 begins ******************************/
    /********************* Vibrato and Percussion controls *******************/
    Rectangle<int> row_1(area.removeFromTop(row_1_height));

    vibrato_lower.setBounds(row_1.removeFromLeft(row_width/10).reduced(triple_margin));
    vibrato_upper.setBounds(row_1.removeFromLeft(row_width/10).reduced(triple_margin));

    Rectangle<int> vibrato_chorus_area(row_1.removeFromLeft(row_width*4/10).reduced(margin));
    vibrato_chorus_area.removeFromLeft(double_margin)
                       .removeFromRight(double_margin);
    int vibrato_chorus_area_width = vibrato_chorus_area.getWidth();
    vibrato_chorus_area.removeFromTop(row_3_height/6);
    vibrato_chorus.setBounds(vibrato_chorus_area.removeFromTop(row_1_height*2/6));
    vibrato_chorus_label.setBounds(vibrato_chorus_area.removeFromBottom(row_1_height/6));
    vibrato_chorus_area.removeFromBottom(row_1_height/6);
    C1_label.setBounds(vibrato_chorus_area.removeFromLeft(vibrato_chorus_area_width/6));
    V2_label.setBounds(vibrato_chorus_area.removeFromLeft(vibrato_chorus_area_width/6));
    C2_label.setBounds(vibrato_chorus_area.removeFromLeft(vibrato_chorus_area_width/6));
    V3_label.setBounds(vibrato_chorus_area.removeFromLeft(vibrato_chorus_area_width/6));
    C3_label.setBounds(vibrato_chorus_area.removeFromLeft(vibrato_chorus_area_width/6));
    V1_label.setBounds(vibrato_chorus_area.removeFromLeft(vibrato_chorus_area_width/6));

    perc_on_off.setBounds(row_1.removeFromLeft(row_width/10).reduced(triple_margin));
    perc_soft_norm.setBounds(row_1.removeFromLeft(row_width/10).reduced(triple_margin));
    perc_fast_slow.setBounds(row_1.removeFromLeft(row_width/10).reduced(triple_margin));
    perc_2nd_3rd.setBounds(row_1.removeFromLeft(row_width/10).reduced(triple_margin));
    /****************************** Row 1 ends *******************************/


    /***************************** Row 2 begins ******************************/
    /******* Overdrive, Gain, Rotary speaker, Reverb and Volume controls *****/
    Rectangle<int> row_2(area.removeFromTop(row_2_height));

    overdrive.setBounds(row_2.removeFromLeft(row_width/10).reduced(triple_margin));

    Rectangle<int> gain_area(row_2.removeFromLeft(row_width*2/10).reduced(margin));
    gain.setBounds(gain_area.removeFromTop(row_2_height*5/6));
    gain_label.setBounds(gain_area.removeFromTop(row_2_height/6));
    Rectangle<int> rotary_area(row_2.removeFromLeft(row_width*3/10).reduced(margin));
    int rotary_area_width = rotary_area.getWidth();
    rotary_area.removeFromTop(row_2_height/6);
    rotary.setBounds(rotary_area.removeFromTop(row_2_height*2/6));    
    rotary_label.setBounds(rotary_area.removeFromBottom(row_2_height/6));
    rotary_area.removeFromBottom(row_2_height/6);
    rotary_slow_label.setBounds(rotary_area.removeFromLeft(rotary_area_width/3));
    rotary_stop_label.setBounds(rotary_area.removeFromLeft(rotary_area_width/3));
    rotary_fast_label.setBounds(rotary_area.removeFromLeft(rotary_area_width/3));

    Rectangle<int> reverb_area(row_2.removeFromLeft(row_width*2/10).reduced(margin));
    reverb.setBounds(reverb_area.removeFromTop(row_2_height*5/6));
    reverb_label.setBounds(reverb_area.removeFromTop(row_2_height/6));

    Rectangle<int> volume_area(row_2.removeFromLeft(row_width*2/10).reduced(margin));
    volume.setBounds(volume_area.removeFromTop(row_2_height*5/6));
    volume_label.setBounds(volume_area.removeFromTop(row_2_height/6));
    /****************************** Row 2 ends *******************************/


    /***************************** Row 3 begins ******************************/
    /******************************* Drawbars ********************************/
    area.removeFromTop(triple_margin);
    Rectangle<int> row_3(area.removeFromTop(row_3_height));

    // Upper manual
    Rectangle<int> drawbar_upper_area(row_3.removeFromLeft(row_width*9/22).reduced(margin));
    int drawbar_upper_area_width = drawbar_upper_area.getWidth();
    drawbar_upper_label.setBounds(drawbar_upper_area.removeFromTop(row_3_height/10));
    for(int i = 0; i < 9; i++)
    {
        drawbar_upper[i].setBounds(drawbar_upper_area.removeFromLeft(drawbar_upper_area_width/9).reduced(margin));
    }
    row_3.removeFromLeft(row_width/22);

    // Lower manual
    Rectangle<int> drawbar_lower_area(row_3.removeFromLeft(row_width*9/22).reduced(margin));
    int drawbar_lower_area_width = drawbar_lower_area.getWidth();
    drawbar_lower_label.setBounds(drawbar_lower_area.removeFromTop(row_3_height/10));
    for(int i = 0; i < 9; i++)
    {
        drawbar_lower[i].setBounds(drawbar_lower_area.removeFromLeft(drawbar_lower_area_width/9).reduced(margin));
    }
    row_3.removeFromLeft(row_width/22);

    // Pedalboard
    Rectangle<int> drawbar_pedalboard_area(row_3.removeFromLeft(row_width*2/22).reduced(margin));
    int drawbar_pedalboard_area_width = drawbar_pedalboard_area.getWidth();
    drawbar_pedalboard_label.setBounds(drawbar_pedalboard_area.removeFromTop(row_3_height/10));
    for(int i = 0; i < 2; i++)
    {
        drawbar_pedalboard[i].setBounds(drawbar_pedalboard_area.removeFromLeft(drawbar_pedalboard_area_width/2).reduced(margin));
    }
    /****************************** Row 3 ends *******************************/


    /***************** Upper manual, Lower manual, Pedalboard ****************/
    /***************************** Row 4 begins ******************************/
    Rectangle<int> row_4(area.removeFromTop(row_4_height));
    float key_width_upper = (row_width)/(float)n_white_keys_manual;

    midiKeyboardUpperManual.setKeyWidth(key_width_upper-0.05);
    midiKeyboardUpperManual.setBounds(row_4);
    /****************************** Row 4 ends *******************************/


    /***************************** Row 5 begins ******************************/
    Rectangle<int> row_5(area.removeFromTop(row_5_height));
    float key_width_lower = (row_width)/(float)n_white_keys_manual;

    midiKeyboardLowerManual.setKeyWidth(key_width_lower-0.05);
    midiKeyboardLowerManual.setBounds(row_5);
    /****************************** Row 5 ends *******************************/


    /***************************** Row 6 begins ******************************/
    Rectangle<int> row_6(area.removeFromTop(row_6_height));
    float key_width_pedalboard = row_width/(float)n_white_keys_manual;
    int pedalboard_padding = (row_width-key_width_pedalboard*n_white_keys_pedalboard)/2;

    // Column 1    
    row_6.removeFromLeft(pedalboard_padding);

    // Column 2
    midiKeyboardPedalBoard.setKeyWidth(key_width_pedalboard-0.05);
    midiKeyboardPedalBoard.setBounds(row_6.removeFromLeft(key_width_pedalboard*n_white_keys_pedalboard));

    // Column 3
    row_6.removeFromLeft(pedalboard_padding);
    /****************************** Row 6 ends *******************************/
}

void OpenB3AudioProcessorEditor::handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    keyboardState.noteOn(midiChannel, midiNoteNumber, velocity);
}

void OpenB3AudioProcessorEditor::handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    keyboardState.noteOn(midiChannel, midiNoteNumber, velocity);
}

void OpenB3AudioProcessorEditor::init_control_board()
{
    /**** Vibrato ****/
    vibrato_lower.setClickingTogglesState(true);
    vibrato_lower.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::darkred);
    vibrato_lower.setButtonText("ON\n\n"
                                "Vibr.\n"
                                "Lower\n\n"
                                "OFF");
    addAndMakeVisible(vibrato_lower);

    vibrato_upper.setClickingTogglesState(true);
    vibrato_upper.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::darkred);
    vibrato_upper.setButtonText("ON\n\n"
                                "Vibr.\n"
                                "Upper\n\n"
                                "OFF");
    addAndMakeVisible(vibrato_upper);

    /**** Vibrato and chorus ****/
    vibrato_chorus_label.setText("Vibrato & Chorus", juce::NotificationType::dontSendNotification);
    vibrato_chorus_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(vibrato_chorus_label);

    C1_label.setText("C1", juce::NotificationType::dontSendNotification);
    C1_label.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(C1_label);
    V2_label.setText("V2", juce::NotificationType::dontSendNotification);
    V2_label.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(V2_label);
    C2_label.setText("C2", juce::NotificationType::dontSendNotification);
    C2_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(C2_label);
    V3_label.setText("V3", juce::NotificationType::dontSendNotification);
    V3_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(V3_label);
    C3_label.setText("C3", juce::NotificationType::dontSendNotification);
    C3_label.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(C3_label);
    V1_label.setText("V1", juce::NotificationType::dontSendNotification);
    V1_label.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(V1_label);

    vibrato_chorus.setRange(0, 5, 1);
    vibrato_chorus.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    //vibrato_chorus.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    //vibrato_chorus.setRotaryParameters(0, 2*M_PI, false);
    vibrato_chorus.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    vibrato_chorus.setTitle("Vibrato and Chorus");
    addAndMakeVisible(vibrato_chorus);

    /**** Percussion ****/
    perc_on_off.setClickingTogglesState(true);
    perc_on_off.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::darkred);
    perc_on_off.setButtonText("ON\n\n"
                              "Perc.\n\n"
                              "OFF");
    addAndMakeVisible(perc_on_off);

    perc_soft_norm.setClickingTogglesState(true);
    perc_soft_norm.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::darkred);
    perc_soft_norm.setButtonText("SOFT\n\n"
                              "Perc.\n\n"
                              "NORM.");
    addAndMakeVisible(perc_soft_norm);

    perc_fast_slow.setClickingTogglesState(true);
    perc_fast_slow.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::darkred);
    perc_fast_slow.setButtonText("FAST\n\n"
                              "Perc.\n\n"
                              "SLOW");
    addAndMakeVisible(perc_fast_slow);

    perc_2nd_3rd.setClickingTogglesState(true);
    perc_2nd_3rd.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::darkred);
    perc_2nd_3rd.setButtonText("2ND\n\n"
                              "Perc.\n\n"
                              "3RD");
    addAndMakeVisible(perc_2nd_3rd);

    /**** Overdrive ****/
    overdrive.setClickingTogglesState(true);
    overdrive.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::darkred);
    overdrive.setButtonText("ON\n\n"
                            "Over\n"
                            "drive\n\n"
                            "OFF");
    addAndMakeVisible(overdrive);

    /**** Gain ****/
    gain_label.setText("Gain", juce::NotificationType::dontSendNotification);
    gain_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gain_label);

    gain.setRange(0, 1);
    gain.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    gain.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(gain);

    /**** Rotary speaker control ****/
    rotary_label.setText("Rotary Speaker", juce::NotificationType::dontSendNotification);
    rotary_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(rotary_label);

    rotary_slow_label.setText("SLOW", juce::NotificationType::dontSendNotification);
    rotary_slow_label.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(rotary_slow_label);
    rotary_stop_label.setText("STOP", juce::NotificationType::dontSendNotification);
    rotary_stop_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(rotary_stop_label);
    rotary_fast_label.setText("FAST", juce::NotificationType::dontSendNotification);
    rotary_fast_label.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(rotary_fast_label);

    rotary.setRange(0, 2, 1);
    rotary.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    rotary.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    rotary.setTitle("Rotary Speaker Control");
    addAndMakeVisible(rotary);

    /**** Reverb ****/
    reverb_label.setText("Reverb", juce::NotificationType::dontSendNotification);
    reverb_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverb_label);

    reverb.setRange(0, 1);
    reverb.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    reverb.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    reverb.setValue(0.2);
    addAndMakeVisible(reverb);

    /**** Volume ****/
    volume_label.setText("Volume", juce::NotificationType::dontSendNotification);
    volume_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(volume_label);

    //volume.setRange(0, 1);
    volume.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    volume.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    //volume.setValue(0.8);
    addAndMakeVisible(volume);
}

void OpenB3AudioProcessorEditor::init_drawbars()
{
    // Upper manual
    drawbar_upper_label.setText("Upper Manual", juce::NotificationType::dontSendNotification);
    drawbar_upper_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(drawbar_upper_label);
    for(int i = 0; i < 9; i++)
    {
        drawbar_upper[i].setRange(0, 8, 1);
        drawbar_upper[i].setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        drawbar_upper[i].setTextBoxStyle(juce::Slider::TextBoxBelow, true,
                                         drawbar_upper[i].getTextBoxWidth(), drawbar_upper[i].getTextBoxHeight());
        drawbar_upper[i].setTitle("Upper Manual Drawbar");
        addAndMakeVisible(drawbar_upper[i]);
    }

    // Lower manual
    drawbar_lower_label.setText("Lower Manual", juce::NotificationType::dontSendNotification);
    drawbar_lower_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(drawbar_lower_label);
    for(int i = 0; i < 9; i++)
    {
        drawbar_lower[i].setRange(0, 8, 1);
        drawbar_lower[i].setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        drawbar_lower[i].setTextBoxStyle(juce::Slider::TextBoxBelow, true,
                                         drawbar_lower[i].getTextBoxWidth(), drawbar_lower[i].getTextBoxHeight());
        drawbar_lower[i].setTitle("lower Manual Drawbar");
        addAndMakeVisible(drawbar_lower[i]);
    }

    // Pedalboard
    drawbar_pedalboard_label.setText("Pedalboard", juce::NotificationType::dontSendNotification);
    drawbar_pedalboard_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(drawbar_pedalboard_label);
    for(int i = 0; i < 2; i++)
    {
        drawbar_pedalboard[i].setRange(0, 8, 1);
        drawbar_pedalboard[i].setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        drawbar_pedalboard[i].setTextBoxStyle(juce::Slider::TextBoxBelow, true,
                                         drawbar_pedalboard[i].getTextBoxWidth(), drawbar_pedalboard[i].getTextBoxHeight());
        drawbar_pedalboard[i].setTitle("pedalboard Manual Drawbar");
        addAndMakeVisible(drawbar_pedalboard[i]);
    }
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

void OpenB3AudioProcessorEditor::create_attachments()
{
    vibrato_upper_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
            (audioProcessor.apvts, "VIBRATO UPPER", vibrato_upper);
    vibrato_lower_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
            (audioProcessor.apvts, "VIBRATO LOWER", vibrato_lower);

    vibrato_chorus_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
            (audioProcessor.apvts, "VIBRATO_CHORUS", vibrato_chorus);

    perc_on_off_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
            (audioProcessor.apvts, "PERC_ON_OFF", perc_on_off);
    perc_soft_norm_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
            (audioProcessor.apvts, "PERC_SOFT_NORM", perc_soft_norm);
    perc_fast_slow_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
            (audioProcessor.apvts, "PERC_FAST_SLOW", perc_fast_slow);
    perc_2nd_3rd_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
            (audioProcessor.apvts, "PERC_2ND_3RD", perc_2nd_3rd);

    overdrive_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
            (audioProcessor.apvts, "OVERDRIVE", overdrive);
    gain_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
            (audioProcessor.apvts, "GAIN", gain);

    rotary_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
            (audioProcessor.apvts, "ROTARY", rotary);

    reverb_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
            (audioProcessor.apvts, "REVERB", reverb);

    volume_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
            (audioProcessor.apvts, "VOLUME", volume);
}
