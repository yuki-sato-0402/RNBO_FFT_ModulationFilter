#include "CustomAudioProcessor.h"
#include "CustomAudioEditor.h"

CustomAudioEditor::CustomAudioEditor (CustomAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), valueTreeState(vts)
{
    lightLookAndFeel.setColourScheme(juce::LookAndFeel_V4::getLightColourScheme());
    
    addAndMakeVisible(buttonA);
    addAndMakeVisible(buttonB);

    buttonAattachment.reset (new ButtonAttachment (valueTreeState, "multype", buttonA));
    buttonA.onClick = [this] { updateToggleState (&buttonA,   "filterTypeA");  };
    buttonB.onClick = [this] { updateToggleState (&buttonB,   "filterTypeB"); };
    buttonA.setRadioGroupId (FilterButtons);
    buttonB.setRadioGroupId (FilterButtons);

    addAndMakeVisible(dial1Slider);
    dial1Attachment.reset (new SliderAttachment (valueTreeState, "direction", dial1Slider));
    dial1Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dial1Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial1Slider.getTextBoxWidth(), dial1Slider.getTextBoxHeight());
    dial1Slider.setLookAndFeel(&lightLookAndFeel);
    dial1Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);


    addAndMakeVisible(label1);
    label1.setText ("direction", juce::dontSendNotification);
    label1.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(dial2Slider);
    dial2Attachment.reset (new SliderAttachment (valueTreeState, "dutyCycle", dial2Slider));
    dial2Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dial2Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial2Slider.getTextBoxWidth(), dial2Slider.getTextBoxHeight());
    dial2Slider.setLookAndFeel(&lightLookAndFeel);
    dial2Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);

    addAndMakeVisible(label2);
    label2.setText ("dutyCycle", juce::dontSendNotification);
    label2.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(dial3Slider);
    dial3Attachment.reset (new SliderAttachment (valueTreeState, "freqOffset", dial3Slider));
    dial3Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);     
    dial3Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial3Slider.getTextBoxWidth(), dial3Slider.getTextBoxHeight());
    dial3Slider.setLookAndFeel(&lightLookAndFeel);
    dial3Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);

    addAndMakeVisible(label3);
    label3.setText ("freq", juce::dontSendNotification);
    label3.setJustificationType(juce::Justification::centred);
    
    setSize(400, 300);
}


void CustomAudioEditor::paint (Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void CustomAudioEditor::resized()
{
    auto area = getLocalBounds();

    const int componentWidth1 = (area.getWidth() - 40)/3;
   
    const int componentHeight = (area.getHeight() - 60) / 3;
    const int padding = 20;         

    buttonA.setBounds(padding, padding, componentWidth1 , componentHeight);
    buttonB.setBounds(buttonA.getRight(), padding, componentWidth1 , componentHeight);

    dial1Slider.setBounds(padding,  buttonA.getBottom() +padding , componentWidth1 ,  componentHeight * 2);
    dial2Slider.setBounds(dial1Slider.getRight() ,  buttonA.getBottom() + padding, componentWidth1 ,  componentHeight * 2);
    dial3Slider.setBounds(dial2Slider.getRight(),  buttonA.getBottom()+ padding, componentWidth1 ,  componentHeight * 2);

    label1.setBounds(dial1Slider.getX(), dial1Slider.getY(), dial1Slider.getWidth(),dial1Slider.getTextBoxHeight() );
    label2.setBounds(dial2Slider.getX(), dial2Slider.getY(), dial2Slider.getWidth(),dial2Slider.getTextBoxHeight() );
    label3.setBounds(dial3Slider.getX(), dial3Slider.getY(), dial3Slider.getWidth(),dial3Slider.getTextBoxHeight() );
}