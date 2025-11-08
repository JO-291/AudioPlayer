
#include "PlayerAudio.h"

PlayerAudio::PlayerAudio() { formatManager.registerBasicFormats(); }

PlayerAudio::~PlayerAudio() {
    transportSource.stop();
    transportSource.setSource(nullptr);
    audioSourcePlayer.setSource(nullptr);
    resamplerSource.reset();
    readerSource.reset();
}

void PlayerAudio::openFile() {
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select audio files...", juce::File{}, "*.wav;*.mp3;*.aiff");

    fileChooser->launchAsync(juce::FileBrowserComponent::openMode
                            | juce::FileBrowserComponent::canSelectFiles
                            | juce::FileBrowserComponent::canSelectMultipleItems,
        [this](const juce::FileChooser& fc) {
            auto results = fc.getResults();
            if (results.isEmpty()) return;

            bool playlistChanged = false;
            for (const auto& file : results) {
                if (file.existsAsFile()) { playlist.push_back(file); playlistChanged = true; }
            }

            if (playlistChanged) {
                if (currentTrackIndex == -1 || !hasSource()) {
                    // Load the first of the newly added files (don’t auto-play)
                    loadTrack((int)playlist.size() - results.size(), false);
                }
                sendChangeMessage();
            }
        });
}

void PlayerAudio::loadTrack(int trackIndex, bool startPlaying) {
    transportSource.stop();
    juce::MessageManager::callAsync([]{});
    if (trackIndex < 0 || trackIndex >= (int)playlist.size()) return;
    audioSourcePlayer.setSource(nullptr);
    transportSource.stop();
    playing = false;
    sendChangeMessage();
    currentTrackIndex = trackIndex;
    auto fileToLoad = playlist[(size_t)trackIndex];

    audioSourcePlayer.setSource(nullptr);
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
    resamplerSource.reset();

    if (auto* reader = formatManager.createReaderFor(fileToLoad)) {
        originalFileSampleRate = reader->sampleRate;
        readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

        resamplerSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false, reader->numChannels);

        // Prepare with current device I/O timing if already known
        if (currentBlockSize > 0 && currentSampleRate > 0.0)
            resamplerSource->prepareToPlay(currentBlockSize, currentSampleRate);

        // IMPORTANT: set speed AFTER prepareToPlay so currentSampleRate is known
        setSpeed(speed);

        audioSourcePlayer.setSource(resamplerSource.get());
        currentFileName = fileToLoad.getFileName();

        if (startPlaying) {
            transportSource.start();
            playing = true;
        }
        else  { playing = false; }

        sendChangeMessage();
    }
    if (currentSampleRate > 0 && currentBlockSize > 0)
    {
        resamplerSource->prepareToPlay(currentBlockSize, currentSampleRate);
        setSpeed(speed);
    }

}

void PlayerAudio::loadNewReader(juce::File& file) {
    // (Kept for reference; not typically used by UI anymore)
    audioSourcePlayer.setSource(nullptr);
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
    resamplerSource.reset();

    if (auto* reader = formatManager.createReaderFor(file)) {
        originalFileSampleRate = reader->sampleRate;
        readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
        resamplerSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false, reader->numChannels);

        if (currentBlockSize > 0 && currentSampleRate > 0.0)
            resamplerSource->prepareToPlay(currentBlockSize, currentSampleRate);

        setSpeed(speed);
        audioSourcePlayer.setSource(resamplerSource.get());
        currentFileName = file.getFileName();
    }
}

juce::AudioSource* PlayerAudio::getAudioSource() { return resamplerSource.get(); }

void PlayerAudio::releaseAudioResources() {
    if (resamplerSource) resamplerSource->releaseResources();
}

bool PlayerAudio::playPauseToggle() {
    if (transportSource.isPlaying()) { transportSource.stop(); playing = false; }
    else                             { transportSource.start(); playing = true; }
    return playing;
}

void PlayerAudio::setToStart() { transportSource.setPosition(0.0); }
void PlayerAudio::setToStart(bool startPlaying) { setToStart(); if (startPlaying) transportSource.start(); }
void PlayerAudio::setToEnd() { transportSource.setPosition(getLengthInSeconds() - 0.01); }
void PlayerAudio::setPosition(double newPosition) { transportSource.setPosition(newPosition); }

void PlayerAudio::setGain(float newGain) {
    gain = newGain; if (!muted) transportSource.setGain(gain);
}
void PlayerAudio::mute()   { muted = true;  transportSource.setGain(0.0f); }
void PlayerAudio::unmute() { muted = false; transportSource.setGain(gain); }

// void PlayerAudio::setSpeed(double newSpeed) {
//     speed = newSpeed;
//     if (resamplerSource) {
//         // FIX: Correct resampling math. Normal playback ratio is
//         // (originalFileSampleRate / currentSampleRate). Multiply by speed.
//         double ratio = 1.0;
//         if (currentSampleRate > 0.0 && originalFileSampleRate > 0.0)
//             ratio = speed * (originalFileSampleRate / currentSampleRate);
//         else
//             ratio = speed; // fallback until device is prepared
//
//         resamplerSource->setResamplingRatio(ratio);
//     }
// }

void PlayerAudio::setSpeed(double newSpeed)
{
    speed = newSpeed;

    if (resamplerSource)
    {
        if (currentSampleRate > 0 && originalFileSampleRate > 0)
        {
            // ✅ المعادلة الصحيحة تمامًا:
            double ratio = (originalFileSampleRate / currentSampleRate) * speed;
            resamplerSource->setResamplingRatio(ratio);
        }
        else resamplerSource->setResamplingRatio(speed);

    }
}


void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    currentBlockSize = samplesPerBlockExpected;
    currentSampleRate = sampleRate;

    audioSourcePlayer.prepareToPlay(samplesPerBlockExpected, sampleRate);

    if (resamplerSource) {
        resamplerSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
        setSpeed(speed); // refresh ratio now that sampleRate is known
    }
}

void PlayerAudio::nextTrack(bool startPlaying) {
    if (playlist.empty()) return;
    int nextIndex = (currentTrackIndex < 0) ? 0 : currentTrackIndex + 1;
    if (nextIndex >= (int)playlist.size()) nextIndex = 0;
    loadTrack(nextIndex, startPlaying);
}

// Getters
double PlayerAudio::getCurrentPosition() const { return transportSource.getCurrentPosition(); }
double PlayerAudio::getLengthInSeconds() const { return transportSource.getLengthInSeconds(); }
juce::String PlayerAudio::getFileName() const  { return currentFileName; }
bool PlayerAudio::hasSource() const            { return readerSource != nullptr; }
bool PlayerAudio::isPlaying() const            { return transportSource.isPlaying(); }
float PlayerAudio::getGain() const             { return gain; }
bool PlayerAudio::isMuted() const              { return muted; }
int   PlayerAudio::getCurrentTrackIndex() const{ return currentTrackIndex; }
const std::vector<juce::File>& PlayerAudio::getPlaylist() const { return playlist; }
