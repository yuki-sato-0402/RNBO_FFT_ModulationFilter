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
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

private:
    juce::AudioProcessorValueTreeState& valueTreeState; // ✅ Hold by reference

    juce::LookAndFeel_V4 lightLookAndFeel;

    juce::Slider dial1Slider;
    juce::Slider dial2Slider;
    juce::Slider dial3Slider;
    juce::ToggleButton buttonA   { "filterTypeA" },
                       buttonB   { "filterTypeB" };

    juce::Label  label1;
    juce::Label  label2;
    juce::Label  label3;
   
    std::unique_ptr<SliderAttachment> dial1Attachment;
    std::unique_ptr<SliderAttachment> dial2Attachment;
    std::unique_ptr<SliderAttachment> dial3Attachment;
    std::unique_ptr<ButtonAttachment> buttonAattachment;

    void updateToggleState (juce::Button* button, juce::String name)
    {
        auto state = button->getToggleState();
        juce::String stateString = state ? "ON" : "OFF";

        juce::Logger::outputDebugString (name + " Button changed to " + stateString);
    }

    enum RadioButtonIds
    {
        FilterButtons = 1001
    };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioEditor)
};
