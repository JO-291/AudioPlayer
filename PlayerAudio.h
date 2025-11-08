
#pragma once
#include <JuceHeader.h>
#include <vector>

class PlayerAudio : public juce::ChangeBroadcaster {
public:
    PlayerAudio();
    ~PlayerAudio();

    juce::AudioSourcePlayer audioSourcePlayer;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void releaseAudioResources();

    void openFile();
    bool playPauseToggle();
    void mute();
    void unmute();
    void setToStart();
    void setToStart(bool startPlaying);
    void setToEnd();
    void setGain(float newGain);
    void setPosition(double newPosition);
    void setSpeed(double newSpeed);

    double getCurrentPosition() const;
    double getLengthInSeconds() const;
    juce::String getFileName() const;
    bool hasSource() const;
    bool isPlaying() const;
    float getGain() const;
    bool isMuted() const;

    juce::AudioSource* getAudioSource();

    void loadTrack(int trackIndex, bool startPlaying = true);
    void nextTrack(bool startPlaying = true);
    const std::vector<juce::File>& getPlaylist() const;
    int getCurrentTrackIndex() const;

private:
    int currentBlockSize = 0;
    double currentSampleRate = 0.0;
    double originalFileSampleRate = 0.0;

    void loadNewReader(juce::File& file);

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::ResamplingAudioSource> resamplerSource;
    std::unique_ptr<juce::FileChooser> fileChooser;

    std::vector<juce::File> playlist;
    int currentTrackIndex = -1;

    juce::String currentFileName {"No file selected"};
    double speed = 1.0;
    float gain = 0.5f;
    bool muted = false;
    bool playing = false;
};


