
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
  _editorFont->setHeight(16.0f);

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

    MainWindow::EDIT_Undo,
    MainWindow::EDIT_Redo,
    MainWindow::EDIT_Cut,
    MainWindow::EDIT_Copy,
    MainWindow::EDIT_Paste,
    MainWindow::EDIT_SelectAll,

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
  case MainWindow::EDIT_Undo:
    result.setInfo("Undo", "Undo last action", generalCategory, 0);
    result.addDefaultKeypress('z', ModifierKeys::commandModifier);
    break;
  case MainWindow::EDIT_Redo:
    result.setInfo("Redo", "redo last action", generalCategory, 0);
    result.addDefaultKeypress('z', ModifierKeys::commandModifier);
    break;
  case MainWindow::EDIT_Copy:
    result.setInfo("Copy", "copy", generalCategory, 0);
    result.addDefaultKeypress('c', ModifierKeys::commandModifier);
    break;
  case MainWindow::EDIT_Cut:
    result.setInfo("Cut", "cut", generalCategory, 0);
    result.addDefaultKeypress('x', ModifierKeys::commandModifier);
    break;
  case MainWindow::EDIT_Paste:
    result.setInfo("Paste", "paste", generalCategory, 0);
    result.addDefaultKeypress('v', ModifierKeys::commandModifier);
    break;
  case MainWindow::EDIT_SelectAll:
    result.setInfo("Select All", "select all", generalCategory, 0);
    result.addDefaultKeypress('a', ModifierKeys::commandModifier);
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

  case MainWindow::EDIT_Undo:
    if (MainComponent::OpenDocument* doc = currentDoc()) {
        doc->editor->undo();
    }
    break; 
  case MainWindow::EDIT_Redo:
      if (MainComponent::OpenDocument* doc = currentDoc())
        doc->editor->redo();
      break;

  case MainWindow::EDIT_Copy:
    if (MainComponent::OpenDocument* doc = currentDoc())
      doc->editor->copyToClipboard();
    break;
  case MainWindow::EDIT_Paste:
    if (MainComponent::OpenDocument* doc = currentDoc())
      doc->editor->pasteFromClipboard();
    break;
  case MainWindow::EDIT_Cut:
    if (MainComponent::OpenDocument* doc = currentDoc())
      doc->editor->cutToClipboard();
    break;
  case MainWindow::EDIT_SelectAll:
    if (MainComponent::OpenDocument* doc = currentDoc()) {
      doc->editor->selectAll();
    }
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
  arr.add("Edit");
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
  case 1:
    m.addCommandItem(commandManager, MainWindow::EDIT_Undo);
    m.addCommandItem(commandManager, MainWindow::EDIT_Redo);
    m.addSeparator();
    m.addCommandItem(commandManager, MainWindow::EDIT_Cut);
    m.addCommandItem(commandManager, MainWindow::EDIT_Copy);
    m.addCommandItem(commandManager, MainWindow::EDIT_Paste);
    m.addSeparator();
    m.addCommandItem(commandManager, MainWindow::EDIT_SelectAll);
    break;
  }

  return m;
}

void MainComponent::menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) { }

void MainComponent::changeListenerCallback(ChangeBroadcaster *) { }

MainComponent::OpenDocument* MainComponent::currentDoc() {
  int index = _tabs.getCurrentTabIndex();
  if (index < 0)
    return nullptr;

  for (OpenDocsList::iterator it = _opendocs.begin(); it != _opendocs.end(); ++it) {
    if ((*it)->editor == _tabs.getTabContentComponent(index)) {
      return *it;
    }
  }

  return nullptr;
}

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
  File file;
#if JUCE_MAC
    FileChooser myChooser ("Select a file to edit",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*");
    if (myChooser.browseForFileToOpen())
    {
      file = myChooser.getResult();
      
    }
#else
  int dialog_flags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
  WildcardFileFilter wildcard("*.cpp;*.h;*.cc;*.hpp;*.c;*.dart;*.lua;*.txt;*.gn;*.gni;", "*", String());
  String startingFile;
  
  FileBrowserComponent browserComponent(dialog_flags, _settings->getValue("last folder"), 
    &wildcard, nullptr);

  FileChooserDialogBox dialog("Select a file to edit", String(),
    browserComponent, false, browserComponent.findColour(AlertWindow::backgroundColourId));
  if (dialog.show()) {
    file = browserComponent.getSelectedFile(0);
  }
#endif

  if (file.getFullPathName().length()) {
    add_document(file);
    _settings->setValue("last folder", file.getParentDirectory().getFullPathName());
  }
}
