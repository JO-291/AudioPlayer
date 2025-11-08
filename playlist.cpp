
#include "playlist.h"

playlist::playlist(PlayerAudio& audio, const juce::String& title)
    : DocumentWindow(title, juce::Colours::darkgrey, DocumentWindow::closeButton),
      playerAudio(audio)
{
    setUsingNativeTitleBar(true);
    setResizable(true, false);
    centreWithSize(360, 460);

    playlistListBox.setModel(this);
    playlistListBox.setRowHeight(26);
    playlistListBox.setColour(juce::ListBox::backgroundColourId, juce::Colours::darkgrey.darker(0.3f));
    playlistListBox.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);

    loadFilesButton.onClick = [this] { loadFilesClicked(); };

    content = std::make_unique<ContentComponent>(playlistListBox, loadFilesButton);
    setContentOwned(content.release(), true);

    playerAudio.addChangeListener(this);

    playlistListBox.updateContent();
    setVisible(true);
}

playlist::~playlist() {
    playerAudio.removeChangeListener(this);
}

void playlist::closeButtonPressed() {
    setVisible(false);
}

void playlist::loadFilesClicked() {
    playerAudio.openFile();
}

// ---------- ListBoxModel ----------
int playlist::getNumRows() {
    return (int) playerAudio.getPlaylist().size();
}

void playlist::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) {
    const auto& pl = playerAudio.getPlaylist();

    if (rowNumber == playerAudio.getCurrentTrackIndex())      g.fillAll(juce::Colours::deepskyblue.darker(0.3f));
    else if (rowIsSelected)                                    g.fillAll(juce::Colours::grey.withAlpha(0.35f));
    else                                                       g.fillAll(juce::Colours::darkgrey.darker(0.35f));

    if ((size_t)rowNumber < pl.size()) {
        g.setColour(juce::Colours::white);
        g.drawText(pl[(size_t)rowNumber].getFileName(),
                   juce::Rectangle<int>(8, 0, width - 16, height),
                   juce::Justification::centredLeft, true);
    }
}

void playlist::listBoxItemClicked(int rowNumber, const juce::MouseEvent&) {
    playerAudio.loadTrack(rowNumber, true);
}

// ---------- ChangeListener ----------
void playlist::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == &playerAudio) {
        playlistListBox.updateContent();
        playlistListBox.repaint();
        const int idx = playerAudio.getCurrentTrackIndex();
        if (idx >= 0) {
            playlistListBox.selectRow(idx);
          //  playlistListBox.scrollToEnsureRowIsVisible(idx);
        }
    }
}

