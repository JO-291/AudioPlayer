#include<string>
#include "PlayerGUI.h"
#include "MainComponent.h"
#include <sstream>

PlayerGUI::PlayerGUI(PlayerAudio& audio, MainComponent& parentComponent)
    : playerAudio(audio), mainComponent(parentComponent)
{
    startTimerHz(30);


    addAndMakeVisible(openButton);
    addAndMakeVisible(playButton);
    addAndMakeVisible(fromstartb);
    addAndMakeVisible(toendbutton);
    addAndMakeVisible(volumeSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(progressSlider);
    addAndMakeVisible(nameLabel);
    addAndMakeVisible(timelabel);
    addAndMakeVisible(timelaplabel);
    addAndMakeVisible(playlistButton);
    addAndMakeVisible(mute);
    addAndMakeVisible(loop);
    addAndMakeVisible(p10);
    addAndMakeVisible(m10);


    openButton.addListener(this);
    playButton.addListener(this);
    fromstartb.addListener(this);
    toendbutton.addListener(this);
    volumeSlider.addListener(this);
    mute.addListener(this);
    loop.addListener(this);
    p10.addListener(this);
    m10.addListener(this);
    speedSlider.addListener(this);
    playlistButton.addListener(this);
    // (إعدادات السلايدرز - البَكَر)
    volumeSlider.setRange(0.0, 100.0, 1.0);
    volumeSlider.setValue(50);
    volumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);

    speedSlider.setRange(0.5, 2, 0.01);
    speedSlider.setValue(1);
    speedSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);

    progressSlider.setRange(0.0, 1.0, 0.001);
    progressSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    progressSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    progressSlider.onDragStart = [this]() { userIsDraggingSlider = true; };
    progressSlider.onDragEnd = [this]() {
        userIsDraggingSlider = false;
        double newPos = progressSlider.getValue() * playerAudio.getLengthInSeconds();
        playerAudio.setPosition(newPos);
    };

    // (الرموز المبدئية للأزرار)
    openButton.setButtonText("Open File");
    playButton.setButtonText(juce::CharPointer_UTF8("▶"));
    fromstartb.setButtonText(juce::CharPointer_UTF8("⏮"));
    toendbutton.setButtonText(juce::CharPointer_UTF8("⏭"));
    mute.setButtonText(juce::CharPointer_UTF8("🔊"));
    loop.setButtonText(juce::CharPointer_UTF8("🔁"));
    p10.setButtonText(juce::CharPointer_UTF8("⏩"));
    m10.setButtonText(juce::CharPointer_UTF8("⏪"));
}

std::string PlayerGUI::tostring(double seconds) {
    int H = seconds / 3600;
    if (H < 1) H = 0;
    seconds -= H * 3600;
    int M = seconds / 60;
    seconds -= M * 60;
    int S = seconds;
    std::stringstream ss;
    if (H > 0) ss << H << ":";
    if (M < 10) ss << "0" << M;
    else ss << M;
    ss << ":";
    if (S < 10) ss << "0" << S;
    else ss << S;
    return ss.str();
}

PlayerGUI::~PlayerGUI(){ stopTimer(); }
void PlayerGUI::paint (juce::Graphics& g){}
void PlayerGUI::resized()
{
    const int kPadding = 10;
    const int kKnobSize = 80;
    const int kRowHeight = 40;
    const int kButtonSize = 50;

    auto bounds = getLocalBounds().reduced(kPadding);

    // ==== Row 1: Open + Playlist + Track Name ====
    auto topRow = bounds.removeFromTop(kRowHeight);
    openButton.setBounds(topRow.removeFromLeft(100));
    playlistButton.setBounds(topRow.removeFromLeft(100));
    nameLabel.setBounds(topRow.withTrimmedLeft(kPadding));

    bounds.removeFromTop(kPadding);

    // ==== Row 2: Volume + Speed + Progress ====
    auto row2 = bounds.removeFromTop(kKnobSize + kPadding);
    auto knobsArea = row2.removeFromRight(kKnobSize * 2 + kPadding);
    volumeSlider.setBounds(knobsArea.removeFromLeft(kKnobSize));
    speedSlider.setBounds(knobsArea.removeFromRight(kKnobSize));

    auto progressArea = row2;
    progressArea.removeFromTop(kPadding * 2);
    timelaplabel.setBounds(progressArea.removeFromLeft(60));
    timelabel.setBounds(progressArea.removeFromRight(60));
    progressSlider.setBounds(progressArea);

    bounds.removeFromTop(kPadding);

    // ==== Row 3: Transport Buttons ====
    auto row3 = bounds.removeFromTop(kButtonSize);
    row3.reduce(kPadding, 0);
    m10.setBounds(row3.removeFromLeft(kButtonSize));
    fromstartb.setBounds(row3.removeFromLeft(kButtonSize));
    playButton.setBounds(row3.removeFromLeft(kButtonSize + 20));
    toendbutton.setBounds(row3.removeFromLeft(kButtonSize));
    p10.setBounds(row3.removeFromLeft(kButtonSize));

    bounds.removeFromTop(kPadding);

    // ==== Row 4: Loop + Mute ====
    auto row4 = bounds.removeFromTop(kButtonSize);
    row4.reduce(kPadding, 0);
    loop.setBounds(row4.removeFromLeft(kButtonSize+10));
    mute.setBounds(row4.removeFromLeft(kButtonSize+20));
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &openButton) playerAudio.openFile();

    else if (button == &p10 && playerAudio.getCurrentPosition()+10 < playerAudio.getLengthInSeconds())  playerAudio.setPosition(playerAudio.getCurrentPosition()+10) ;

    else if (button == &m10 && playerAudio.getCurrentPosition()-10 > 0) playerAudio.setPosition(playerAudio.getCurrentPosition()-10) ;

    else if (button == &loop) {
        if (looping) {
            looping = false;
            loop.setButtonText(juce::CharPointer_UTF8("🔁"));
        }
        else {
            looping = true;
            loop.setButtonText(juce::CharPointer_UTF8("🔂"));
        }
    }
    else if (button == &mute) {
        if (playerAudio.isMuted()) {
            playerAudio.unmute();
            mute.setButtonText(juce::CharPointer_UTF8("🔊"));
        }
        else {
            playerAudio.mute();
            mute.setButtonText(juce::CharPointer_UTF8("🔇"));
        }
    }
    else if (button == &playButton) {
        if (!playerAudio.hasSource())playerAudio.openFile();
        else{
            if (playerAudio.playPauseToggle())playButton.setButtonText(juce::CharPointer_UTF8("⏸"));
            else playButton.setButtonText(juce::CharPointer_UTF8("▶"));
        }
    }
    else if (button == &openButton) {
        playerAudio.openFile();
    }
    else if (button == &playlistButton) {
        mainComponent.showPlaylist(playerAudio);
    }
    else if (button == &toendbutton)
    {
        playerAudio.setToEnd();
        playButton.setButtonText(juce::CharPointer_UTF8("▶"));
    }
    else if (button == &fromstartb)
    {
        playerAudio.setToStart();
        playButton.setButtonText(juce::CharPointer_UTF8("▶"));
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)playerAudio.setGain((float)slider->getValue() / 100);
    if (slider == &speedSlider)playerAudio.setSpeed(slider->getValue());
}

void PlayerGUI::timerCallback() {
    timelabel.setText(tostring(playerAudio.getLengthInSeconds()), juce::dontSendNotification);
    timelaplabel.setText(tostring(playerAudio.getCurrentPosition()), juce::dontSendNotification);
    nameLabel.setText(playerAudio.getFileName(), juce::dontSendNotification);

    if (!userIsDraggingSlider && playerAudio.getLengthInSeconds() > 0){
        double progress = playerAudio.getCurrentPosition() / playerAudio.getLengthInSeconds();
        progressSlider.setValue(progress, juce::dontSendNotification);
    }
    if (userIsDraggingSlider && playerAudio.getLengthInSeconds() > 0){
        timelaplabel.setText(tostring(progressSlider.getValue() * playerAudio.getLengthInSeconds()), juce::dontSendNotification);
    }

    if(playerAudio.hasSource() && playerAudio.getLengthInSeconds() > 0 &&
       (playerAudio.getLengthInSeconds() - playerAudio.getCurrentPosition() < 0.3))
    {
        if (looping) {
            playerAudio.setToStart(true);
        }
        else {
            playerAudio.setToStart();
            playButton.setButtonText(juce::CharPointer_UTF8("▶"));
        }
    }
    if(playerAudio.hasSource() && playerAudio.getLengthInSeconds() > 0 &&
       (playerAudio.getLengthInSeconds() - playerAudio.getCurrentPosition() < 0.3))
    {
        if (looping) {
            playerAudio.setToStart(true);
        }
        else {
            // ⬅️ Go to the next song!
            playerAudio.nextTrack(true);
        }
    }
}