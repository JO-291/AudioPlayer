#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include <string>

class MainComponent;

class PlayerGUI  : public juce::Component,
                   public juce::Slider::Listener,
                   public juce::Button::Listener,
                   public juce::Timer
{
public:
    PlayerGUI(PlayerAudio& audio, MainComponent& parentComponent);
    ~PlayerGUI() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void buttonClicked (juce::Button* button) override;
    void sliderValueChanged (juce::Slider* slider) override;
    void timerCallback() override;

private:
    PlayerAudio& playerAudio;
    std::string tostring(double seconds);

    juce::Slider volumeSlider;
    juce::Slider progressSlider;
    juce::Slider speedSlider;
    juce::TextButton fromstartb{juce::CharPointer_UTF8("⏮")};
    juce::TextButton toendbutton{juce::CharPointer_UTF8("⏭")};
    juce::TextButton openButton { "Open File" };
    juce::TextButton playButton { juce::CharPointer_UTF8("▶") };
    juce::TextButton mute { juce::CharPointer_UTF8("🔊") };
    juce::TextButton loop { juce::CharPointer_UTF8("🔁") };
    juce::TextButton p10 { juce::CharPointer_UTF8("⏩") };
    juce::TextButton m10 { juce::CharPointer_UTF8("⏪") };
    juce::TextButton playlistButton {"List"};
    juce::Label nameLabel;
    juce::Label timelabel;
    juce::Label timelaplabel;
    juce::ListBox playlistListBox;

    bool looping=false;
    bool userIsDraggingSlider = false;
    MainComponent& mainComponent;
};