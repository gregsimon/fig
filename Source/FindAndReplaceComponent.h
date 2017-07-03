
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class FindAndReplaceComponent    : public Component, public TextEditor::Listener
{
public:
    FindAndReplaceComponent();
    ~FindAndReplaceComponent();

    class Listener {
    public:
        virtual ~Listener() {}
        virtual void returnKeyPressed(const String&) = 0;
    };
    void addListener(Listener* newListener) {
      _listeners.add(newListener);
    }
    void removeListener(Listener* newListener) {
      _listeners.remove(newListener);
    }


    void paint (Graphics&) override;
    void resized() override;

    void focus();

    const String text() { return _findText.getText(); }

    virtual void textEditorReturnKeyPressed(TextEditor &) override;
    virtual void textEditorEscapeKeyPressed(TextEditor &) override;

private:
    Label _findLabel;
    TextEditor _findText;
    ListenerList<Listener> _listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FindAndReplaceComponent)
};
