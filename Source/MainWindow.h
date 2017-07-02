

#pragma once

class MainComponent;

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
    FILE_SaveAs
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

  void handleAsyncUpdate() override;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
