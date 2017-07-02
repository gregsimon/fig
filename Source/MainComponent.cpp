
#include "../JuceLibraryCode/JuceHeader.h"
#include "MainWindow.h"
#include "MainComponent.h"



MainComponent::MainComponent()
  : _tabs(TabbedButtonBar::Orientation::TabsAtTop)
{
  _options.applicationName = "Gooey";
  _options.commonToAllUsers = false;
  _options.filenameSuffix = "xml";
  _options.folderName = "GregsTools";
  _options.storageFormat = PropertiesFile::storeAsXML;
  _options.ignoreCaseOfKeyNames = true;
  _options.osxLibrarySubFolder = "Application Support";
  _applicationProperties.setStorageParameters(_options);
  _settings = _applicationProperties.getUserSettings();

  // load the font

  int dataSize = 0;
  const char* data = BinaryData::getNamedResource("InconsolataRegular_ttf", dataSize);
  Typeface::Ptr font_data = Typeface::createSystemTypefaceFor(data, dataSize);
  _editorFont = new Font(font_data);
  _editorFont->setHeight(20.0f);

  // set up commands
  // TODO

  // menu bar
  _menu = new MenuBarComponent(this);
  setApplicationCommandManagerToWatch(&MainWindow::getApplicationCommandManager());
#if JUCE_MAC
  MenuBarModel::setMacMainMenu(&_menuModel);
#endif
  //_menu.setModel(&_menuModel);
  addAndMakeVisible(_menu);

  // open document tabs
  addAndMakeVisible(&_tabs);

  // restore window size from settings
  setSize(_settings->getIntValue("win_width", 1024), _settings->getIntValue("win_height", 768));
}

MainComponent::~MainComponent()
{
#if JUCE_MAC
  MenuBarModel::setMacMainMenu(nullptr);
#endif
  PopupMenu::dismissAllActiveMenus();
  _menu->setModel(nullptr);

  for (std::list<OpenDocument*>::iterator it = _opendocs.begin(); it != _opendocs.end(); ++it) {
    delete (*it);
  }

  //delete cmdManager;
}


ApplicationCommandTarget * 	MainComponent::getNextCommandTarget()
{
  return findFirstTargetParentComponent();
}

void MainComponent::getAllCommands(Array< CommandID > &commands)
{
  const CommandID ids[] = { 
    MainWindow::FILE_New,
    MainWindow::FILE_Open,
    MainWindow::FILE_Close,
    MainWindow::FILE_Save,
    MainWindow::FILE_SaveAs,
    MainWindow::FILE_Exit
  };

  commands.addArray(ids, numElementsInArray(ids));
}

void MainComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo &result)
{
  const String generalCategory("General");

  switch (commandID) {
  case MainWindow::FILE_New:
    result.setInfo("New", "Create a new file", generalCategory, 0);
    result.addDefaultKeypress('n', ModifierKeys::commandModifier);
    break;
  case MainWindow::FILE_Open:
    result.setInfo("Open...", "Open a new object to edit", generalCategory, 0);
    result.addDefaultKeypress('o', ModifierKeys::commandModifier);
    break;
  case MainWindow::FILE_Close:
    result.setInfo("Close", "Close currently focused object tab", generalCategory, 0);
    result.addDefaultKeypress('w', ModifierKeys::commandModifier);
    break;
  case MainWindow::FILE_Save:
    result.setInfo("Save", "Save changes", generalCategory, 0);
    result.addDefaultKeypress('s', ModifierKeys::commandModifier);
    break;
  case MainWindow::FILE_SaveAs:
    result.setInfo("Save As...", "Save changes to a new object", generalCategory, 0);
    break;
  case MainWindow::FILE_Exit:
    result.setInfo("Exit", "Exit editor", generalCategory, 0);
    result.addDefaultKeypress('q', ModifierKeys::commandModifier);
    break;
  default:
    return;
  }
}

bool MainComponent::perform(const InvocationInfo &info)
{
  switch (info.commandID) {
  case MainWindow::FILE_Open:
    do_fileopen();
    break;
  case MainWindow::FILE_Close:
    do_fileclose();
    break;
  case MainWindow::FILE_Exit:
    do_exit();
    break;
  }
  return true;
}

void MainComponent::paint (Graphics& )
{
}

void MainComponent::resized()
{
  _menu->setBounds(0, 0, getWidth(), 24);
  _tabs.setBounds(0, 24, getWidth(), getHeight()-48);

  _settings->setValue("win_width", getWidth());
  _settings->setValue("win_height", getHeight());
}

StringArray MainComponent::getMenuBarNames() {
  StringArray arr;
  arr.add("File");
  return arr;
}

PopupMenu MainComponent::getMenuForIndex(int topLevelMenuIndex, const String &) {
  ApplicationCommandManager* commandManager = &MainWindow::getApplicationCommandManager();
  PopupMenu m;
  switch (topLevelMenuIndex) {
  case 0:
    m.addCommandItem(commandManager, MainWindow::FILE_New);
    m.addCommandItem(commandManager, MainWindow::FILE_Open);
    m.addCommandItem(commandManager, MainWindow::FILE_Close);
    m.addSeparator();
    m.addCommandItem(commandManager, MainWindow::FILE_Save);
    m.addCommandItem(commandManager, MainWindow::FILE_SaveAs);
    m.addSeparator();
    m.addCommandItem(commandManager, MainWindow::FILE_Exit);
    break;
  }

  return m;
}

void MainComponent::menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) { }

void MainComponent::changeListenerCallback(ChangeBroadcaster *) { }

void MainComponent::add_document(const File& file)
{
  OpenDocument* doc = new OpenDocument(file);
  _opendocs.push_back(doc);
  
  doc->editor->setFont(*_editorFont);
  _tabs.addTab(file.getFileName(), Colour(66, 67, 65), doc->editor, false);
  _tabs.setCurrentTabIndex(_tabs.getNumTabs() - 1);
  resized();
}

void MainComponent::do_exit()
{
  // unsaved changes?
  for (OpenDocsList::iterator it = _opendocs.begin(); it != _opendocs.end(); ++it) {
    OpenDocument* d = *it;
    if (d->unsaved_changes) {
      // TODO : show dialog box "do you want to save"
    }
  }

  // StandardApplicationCommandIDs::quit
  MainWindow::getApplicationCommandManager().invokeDirectly(StandardApplicationCommandIDs::quit, false);
}

void MainComponent::do_fileclose()
{
  int index = _tabs.getCurrentTabIndex();
  if (index < 0)
    return;
  for (OpenDocsList::iterator it = _opendocs.begin(); it != _opendocs.end(); ++it) {
    if ((*it)->editor == _tabs.getTabContentComponent(index)) {
      _tabs.removeTab(index);
      delete (*it);
      _opendocs.erase(it);
      break;
    }
  }
}

void MainComponent::do_fileopen()
{
#if JUCE_MAC
    FileChooser myChooser ("Select a file to open...",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*");
    if (myChooser.browseForFileToOpen())
    {
      File file (myChooser.getResult());
      add_document(file.loadFileAsString());
    }
#else
  int dialog_flags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
  WildcardFileFilter wildcard("*.cpp;*.h;*.cc;*.hpp;*.c;*.dart;*.lua;*.txt;*.gn;*.gni;", "*", String());
  String startingFile;
  
  FileBrowserComponent browserComponent(dialog_flags, startingFile, &wildcard, nullptr);

  FileChooserDialogBox dialog("Open", String(),
    browserComponent, false, browserComponent.findColour(AlertWindow::backgroundColourId));
  if (dialog.show()) {
    add_document(browserComponent.getSelectedFile(0));
  }
#endif
}
