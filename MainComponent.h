
#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "PlayerGUI.h"

class MainComponent : public juce::AudioAppComponent {
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void showPlaylist(PlayerAudio& targetPlayer);

private:

    std::unique_ptr<juce::Component> playlistWindow;
    PlayerAudio playerAudio1, playerAudio2;
    PlayerGUI   playerGUI1, playerGUI2;

    std::unique_ptr<class playlist> playlistManager1;
    std::unique_ptr<class playlist> playlistManager2;

    juce::AudioBuffer<float> tempBuffer;
    double currentSampleRate = 0.0;
};
