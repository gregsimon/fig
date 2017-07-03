

#pragma once

class MainComponent;
class EditorLookAndFeel;

// --------------------------------------------------------------------------------

class MainWindow : 
  public DocumentWindow, 
  private AsyncUpdater
{
public:
  MainWindow(String name);
  ~MainWindow();

  static ApplicationCommandManager& getApplicationCommandManager();

  enum CommandIDs {
    FILE_New = 0x2000,
    FILE_Open,
    FILE_Close,
    FILE_Save,
    FILE_SaveAs,

    FILE_Recent1,
    FILE_Recent2,
    FILE_Recent3,

    EDIT_Undo,
    EDIT_Redo,
    EDIT_Cut,
    EDIT_Copy,
    EDIT_Paste,
    EDIT_SelectAll,


    FILE_Exit
  };

  void closeButtonPressed() override;

  /* Note: Be careful if you override any DocumentWindow methods - the base
  class uses a lot of them, so by overriding you might break its functionality.
  It's best to do all your work in your content component instead, but if
  you really have to override any DocumentWindow methods, make sure your
  subclass also calls the superclass's method.
  */

private:
  ScopedPointer<MainComponent> mainComponent;
  ScopedPointer<EditorLookAndFeel> darkLookAndFeel;

  void handleAsyncUpdate() override;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
