#include "CustomAudioProcessor.h"
#include "CustomAudioEditor.h"

CustomAudioEditor::CustomAudioEditor (CustomAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), valueTreeState(vts)// 参照メンバーを初期化（必須）
{

    // ルック＆フィールの設定
    lightLookAndFeel.setColourScheme(juce::LookAndFeel_V4::getLightColourScheme());
    
    //buttonA
    addAndMakeVisible(buttonA);
    addAndMakeVisible(buttonB);
   
    std::cout << "multype" << std::endl;
    //スライダーひAPVTSのパラメータを紐づけます。
    buttonAttachment.reset (new ButtonAttachment (valueTreeState, "multype", buttonA));
    buttonBttachment.reset (new ButtonAttachment (valueTreeState, "multype", buttonB));
    addAndMakeVisible(buttonA);
    addAndMakeVisible(buttonB);

    buttonA.setButtonText("Filter TypeA");
    buttonB.setButtonText("Filter TypeB");
    buttonA.setLookAndFeel(&lightLookAndFeel);
    buttonB.setLookAndFeel(&lightLookAndFeel);

    buttonA.setClickingTogglesState(true);
    buttonB.setClickingTogglesState(true);

    buttonA.onClick = [this]() { setSelection(0); };
    buttonB.onClick = [this]() { setSelection(1); };
    setSelection(0); // デフォルトは Option A

  
    //direction
    addAndMakeVisible(dial1Slider);
    std::cout << "direction" << std::endl;
    dial1Attachment.reset (new SliderAttachment (valueTreeState, "direction", dial1Slider));
    dial1Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dial1Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial1Slider.getTextBoxWidth(), dial2Slider.getTextBoxHeight());
    dial1Slider.setLookAndFeel(&lightLookAndFeel);
    // スライダーの目盛りを白色に設定
    dial1Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);


    addAndMakeVisible(label1);
    label1.setText ("direction", juce::dontSendNotification);
    label1.setJustificationType(juce::Justification::centred);
    
    //dutyCycle
    addAndMakeVisible(dial2Slider);
    std::cout << "dutyCycle" << std::endl;
    dial2Attachment.reset (new SliderAttachment (valueTreeState, "dutyCycle", dial2Slider));
    dial2Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dial2Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial2Slider.getTextBoxWidth(), dial2Slider.getTextBoxHeight());
    dial2Slider.setLookAndFeel(&lightLookAndFeel);
    // スライダーの目盛りを白色に設定
    dial2Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);


    addAndMakeVisible(label2);
    label2.setText ("dutyCycle", juce::dontSendNotification);
    label2.setJustificationType(juce::Justification::centred);
    
    //freaqOffset
    addAndMakeVisible(dial3Slider);
    std::cout << "freaqOffset" << std::endl;
    dial3Attachment.reset (new SliderAttachment (valueTreeState, "freaqOffset", dial3Slider));
    dial3Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);     
    dial3Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial3Slider.getTextBoxWidth(), dial3Slider.getTextBoxHeight());
    dial3Slider.setLookAndFeel(&lightLookAndFeel);
    dial3Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);


    addAndMakeVisible(label3);
    label3.setText ("freaq", juce::dontSendNotification);
    label3.setJustificationType(juce::Justification::centred);
    
    setSize(400, 300);
}


void CustomAudioEditor::paint (Graphics& g)
{
    g.fillAll(juce::Colours::black); // 背景を黒に設定
}

void CustomAudioEditor::resized()
{
    //エディター全体の領域を取得
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