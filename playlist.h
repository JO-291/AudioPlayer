// ===============================
// File: playlist.h
// ===============================
#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class playlist : public juce::DocumentWindow,
                 public juce::ListBoxModel,
                 public juce::ChangeListener
{
public:

    playlist(PlayerAudio& audio, const juce::String& title);
    ~playlist() override;

    // ListBoxModel
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int rowNumber, const juce::MouseEvent& e) override;

    // ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // DocumentWindow
    void closeButtonPressed() override;

private:
    // ----- Small content component to layout children (prevents empty popup) -----
    class ContentComponent : public juce::Component {
    public:
        ContentComponent(juce::ListBox& listRef, juce::Button& btnRef) : list(listRef), button(btnRef) {
            addAndMakeVisible(list);
            addAndMakeVisible(button);
        }
        void resized() override {
            auto area = getLocalBounds().reduced(8);
            auto top = area.removeFromTop(32);
            button.setBounds(top.removeFromLeft(120));
            area.removeFromTop(6);
            list.setBounds(area);
        }
    private:
        juce::ListBox& list;
        juce::Button& button;
    };

    PlayerAudio& playerAudio;
    juce::ListBox playlistListBox {"Playlist", this};
    juce::TextButton loadFilesButton {"Load Files"};
    std::unique_ptr<ContentComponent> content;

    void loadFilesClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (playlist)
};

