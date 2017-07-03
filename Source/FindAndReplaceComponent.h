
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class FindAndReplaceComponent    : public Component
{
public:
    FindAndReplaceComponent();
    ~FindAndReplaceComponent();

    void paint (Graphics&) override;
    void resized() override;

    void focus();

private:
    Label _findLabel;
    TextEditor _findText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FindAndReplaceComponent)
};
