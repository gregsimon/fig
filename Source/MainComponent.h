#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "MenuModels.h"

class MainContentComponent   : public Component, public MainMenuModel::Listener
{
public:
  MainContentComponent();
  ~MainContentComponent();

  void paint (Graphics&) override;
  void resized() override;
  void menuItemSelected(int menuItemID) override;

  void do_fileopen();

private:
  CodeEditorComponent* _editor;
  CodeDocument* _codeDocument;
  
  MenuBarComponent _menu;
  ScopedPointer<MainMenuModel> _model;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
