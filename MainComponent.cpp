#include "MainComponent.h"
#include "playlist.h"
MainComponent::MainComponent()
    : playerGUI1(playerAudio1, *this),
      playerGUI2(playerAudio2, *this)
{
    setSize(700, 500);

    addAndMakeVisible(playerGUI1);
    addAndMakeVisible(playerGUI2);

    playlistManager1 = std::make_unique<playlist>(playerAudio1, "Deck 1 Playlist");
    playlistManager2 = std::make_unique<playlist>(playerAudio2, "Deck 2 Playlist");
    playlistManager1->setVisible(false);
    playlistManager2->setVisible(false);
    setAudioChannels(0, 2);
}
void MainComponent::showPlaylist(PlayerAudio& targetPlayer)
{
    if (&targetPlayer == &playerAudio1)playlistManager1->setVisible(true);
    else if (&targetPlayer == &playerAudio2)playlistManager2->setVisible(true);

}
MainComponent::~MainComponent()
{
     shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
    tempBuffer.setSize(2, samplesPerBlockExpected);
    playerAudio1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    playerAudio2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    tempBuffer.setSize(bufferToFill.buffer->getNumChannels(),
                       bufferToFill.numSamples,
                       false, false, false);

    tempBuffer.clear();
    juce::AudioSourceChannelInfo tempChannelInfo(&tempBuffer, 0, bufferToFill.numSamples);

    if (playerAudio1.getAudioSource() != nullptr)
        playerAudio1.getAudioSource()->getNextAudioBlock(bufferToFill);
    else bufferToFill.buffer->clear();

    if (playerAudio2.getAudioSource() != nullptr)playerAudio2.getAudioSource()->getNextAudioBlock(tempChannelInfo);

    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)bufferToFill.buffer->addFrom(channel,bufferToFill.startSample,tempBuffer,channel,0,bufferToFill.numSamples,1.0f);
}

void MainComponent::releaseResources(){
    playerAudio1.releaseAudioResources();
    playerAudio2.releaseAudioResources();

    tempBuffer.setSize(0, 0);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    int y = getHeight() / 2;
    g.setColour(juce::Colours::white);
    g.drawLine(0.0f, (float)y, (float)getWidth(), (float)y, 2.0f);
}


void MainComponent::resized() {
    auto bounds = getLocalBounds();
    playerGUI1.setBounds(bounds.removeFromTop(bounds.getHeight() / 2));
    playerGUI2.setBounds(bounds);

    auto playlistArea = juce::Rectangle<int>(50, 50, 400, 300);

}