
#include "../JuceLibraryCode/JuceHeader.h"
#include "FindAndReplaceComponent.h"

//==============================================================================
FindAndReplaceComponent::FindAndReplaceComponent()
{
  _findLabel.setText("Find:", dontSendNotification);
  _findLabel.setJustificationType(Justification::centredRight);
  addAndMakeVisible(&_findLabel);
  addAndMakeVisible(&_findText);

  _findText.setMultiLine(false, false);
  _findText.setReturnKeyStartsNewLine(false);
  _findText.setScrollbarsShown(false);
  _findText.setSelectAllWhenFocused(true);

  _findText.addListener(this);
}

FindAndReplaceComponent::~FindAndReplaceComponent()
{
}

void FindAndReplaceComponent::focus() {
  _findText.toFront(true);
}

void FindAndReplaceComponent::textEditorReturnKeyPressed(TextEditor &)
{
  _listeners.call(&Listener::returnKeyPressed, _findText.getText());
}

void FindAndReplaceComponent::textEditorEscapeKeyPressed(TextEditor &)
{

}

void FindAndReplaceComponent::paint (Graphics& ) { }

void FindAndReplaceComponent::resized()
{
  const int kFindLabelWidth = 64;
  _findLabel.setBounds(0, 0, kFindLabelWidth, getHeight());
  _findText.setBounds(kFindLabelWidth, 0, getWidth()-kFindLabelWidth, getHeight());
}
