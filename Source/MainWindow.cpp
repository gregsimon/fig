
#include "../JuceLibraryCode/JuceHeader.h"
#include "MainWindow.h"
#include "MainComponent.h"
#include "EditorLookAndFeel.h"

// -------------------------------------------------------------------------
//  MainWindow

static ScopedPointer<ApplicationCommandManager> applicationCommandManager;

MainWindow::MainWindow(String name) : DocumentWindow(name,
  Desktop::getInstance().getDefaultLookAndFeel()
  .findColour(ResizableWindow::backgroundColourId),
  DocumentWindow::allButtons)
{
  
  darkLookAndFeel = new EditorLookAndFeel;
  setLookAndFeel(darkLookAndFeel);
  
  setUsingNativeTitleBar(true);
  mainComponent = new MainComponent();
  mainComponent->setLookAndFeel(darkLookAndFeel);
  setContentOwned(mainComponent, true);

  addKeyListener(getApplicationCommandManager().getKeyMappings());

  centreWithSize(getWidth(), getHeight());
  setResizable(true, true);
  setVisible(true);

  triggerAsyncUpdate();
}

MainWindow::~MainWindow()
{
  applicationCommandManager = nullptr;
  mainComponent = nullptr;
}

void MainWindow::closeButtonPressed()
{
  // This is called when the user tries to close this window. Here, we'll just
  // ask the app to quit when this happens, but you can change this to do
  // whatever you need.
  JUCEApplication::getInstance()->systemRequestedQuit();
}


ApplicationCommandManager& MainWindow::getApplicationCommandManager()
{
  if (applicationCommandManager == nullptr)
    applicationCommandManager = new ApplicationCommandManager();

  return *applicationCommandManager;
}

void MainWindow::handleAsyncUpdate()
{
  // This registers all of our commands with the command manager but has to be done after the window has
  // been created so we can find the number of rendering engines available
  auto& commandManager = MainWindow::getApplicationCommandManager();

  commandManager.registerAllCommandsForTarget(mainComponent);
  commandManager.registerAllCommandsForTarget(JUCEApplication::getInstance());
}


