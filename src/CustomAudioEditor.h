#include "JuceHeader.h"
#include "RNBO.h"

class CustomAudioEditor : public juce::AudioProcessorEditor
{
public:
    CustomAudioEditor(CustomAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);
    ~CustomAudioEditor() override = default;
    void paint (Graphics& g) override;
    void resized() override; 
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    //typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

private:
    juce::AudioProcessorValueTreeState& valueTreeState; // ✅ Hold by reference

    juce::LookAndFeel_V4 lightLookAndFeel;

    juce::Slider dial1Slider;
    juce::Slider dial2Slider;
    juce::Slider dial3Slider;
    juce::TextButton AButton;
    juce::TextButton BButton;

    juce::Label  label1;
    juce::Label  label2;
    juce::Label  label3;
   
    std::unique_ptr<SliderAttachment> dial1Attachment;
    std::unique_ptr<SliderAttachment> dial2Attachment;
    std::unique_ptr<SliderAttachment> dial3Attachment;
    //std::unique_ptr<ButtonAttachment> buttonAattachment;
    //std::unique_ptr<ButtonAttachment> buttonBattachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioEditor)
};
