#include "MainComponent.h"

ApplicationCommandManager* cmdManager = nullptr;


ApplicationCommandTarget * 	MainContentComponent::getNextCommandTarget()
{
  return nullptr;
}

void MainContentComponent::getAllCommands(Array< CommandID > &commands)
{
  commands.add(FILE_New);
  commands.add(FILE_Open);
  commands.add(FILE_Close);
  commands.add(FILE_Save);
  commands.add(FILE_SaveAs);
}

void MainContentComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo &result)
{
  switch (commandID) {
  case FILE_New:
    result.setInfo("New", "Create a new file", "File Menu", 0);
    result.addDefaultKeypress('n', ModifierKeys::ctrlModifier);
    break;
  case FILE_Open:
    result.setInfo("Open...", "Open a new object to edit", "File Menu", 0);
    result.addDefaultKeypress('o', ModifierKeys::ctrlModifier);
    break;
  case FILE_Close:
    result.setInfo("Close", "Close currently focused object tab", "File Menu", 0);
    result.addDefaultKeypress('w', ModifierKeys::ctrlModifier);
    break;
  case FILE_Save:
    result.setInfo("Save", "Save changes", "File Menu", 0);
    result.addDefaultKeypress('s', ModifierKeys::ctrlModifier);
    break;
  case FILE_SaveAs:
    result.setInfo("Save As...", "Save changes to a new object", "File Menu", 0);
    break;
  default:
    return;
  }
}

bool MainContentComponent::perform(const InvocationInfo &info)
{
  switch (info.commandID) {
  case FILE_Open:
    do_fileopen();
    break;
  case FILE_Close:
    do_fileclose();
    break;
  }
  return true;
}

MainContentComponent::MainContentComponent()
: _tabs( TabbedButtonBar::Orientation::TabsAtTop)
{
  cmdManager = new ApplicationCommandManager;
  cmdManager->setFirstCommandTarget(this);

  _options.applicationName = "Gooey";
  _options.commonToAllUsers = false;
  _options.filenameSuffix = "xml";
  _options.folderName = "GregsTools";
  _options.storageFormat = PropertiesFile::storeAsXML;
  _options.ignoreCaseOfKeyNames = true;
  _options.osxLibrarySubFolder = "Application Support";
  _applicationProperties.setStorageParameters(_options);
  _settings = _applicationProperties.getUserSettings();

  cmdManager->registerAllCommandsForTarget(this);

  // load the font
  int dataSize = 0;
  const char* data = BinaryData::getNamedResource("InconsolataRegular_ttf", dataSize);
  Typeface::Ptr font_data = Typeface::createSystemTypefaceFor(data, dataSize);
  _editorFont = new Font(font_data);
  _editorFont->setHeight(20.0f);

  // set up commands
  // TODO
  
  // setup menus
  _model = new MainMenuModel(cmdManager);
  _model->addListener(this);
#if JUCE_MAC
    MenuBarModel::setMacMainMenu(_model);
#endif
  _menu.setModel(_model);
  addAndMakeVisible(&_menu);
  
  // tabs
  addAndMakeVisible(&_tabs);

  // restore window size from settings
  setSize(_settings->getIntValue("win_width", 1024), _settings->getIntValue("win_height", 768));
}

MainContentComponent::~MainContentComponent()
{
  _menu.setModel(nullptr);
#if JUCE_MAC
  MenuBarModel::setMacMainMenu(nullptr);
#endif

  for (std::list<OpenDocument*>::iterator it = _opendocs.begin(); it != _opendocs.end(); ++it) {
    delete (*it);
  }

  delete cmdManager;
}

void MainContentComponent::paint (Graphics& )
{
}

void MainContentComponent::resized()
{
  _menu.setBounds(0, 0, getWidth(), 24);
  _tabs.setBounds(0, 24, getWidth(), getHeight()-48);

  _settings->setValue("win_width", getWidth());
  _settings->setValue("win_height", getHeight());
}

void MainContentComponent::menuItemSelected(int menuItemID)
{
  //switch (menuItemID) {
  //case FILE_Exit:
  //    _menu.setModel(nullptr);
  //  JUCEApplication::getInstance()->perform(ApplicationCommandTarget::InvocationInfo(StandardApplicationCommandIDs::quit));
  //  break;
  //}
}

void MainContentComponent::changeListenerCallback(ChangeBroadcaster *)
{
}

void MainContentComponent::add_document(const File& file)
{
  OpenDocument* doc = new OpenDocument(file);
  _opendocs.push_back(doc);
  
  doc->editor->setFont(*_editorFont);
  _tabs.addTab(file.getFileName(), Colour(66, 67, 65), doc->editor, false);
  resized();
}

void MainContentComponent::do_fileclose()
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

void MainContentComponent::do_fileopen()
{
#if JUCE_MAC
    FileChooser myChooser ("Select a file to open...",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*");
    if (myChooser.browseForFileToOpen())
    {
      File file (myChooser.getResult());
      String contents = file.loadFileAsString();
      _codeDocument->replaceAllContent(contents);
      _tabs.setTabName(0, file.getFileName());
    }
#else
  int dialog_flags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
  WildcardFileFilter wildcard("*.*", "*", String());
  String startingFile;
  
  FileBrowserComponent browserComponent(dialog_flags, startingFile, &wildcard, nullptr);

  FileChooserDialogBox dialog("Open", String(),
    browserComponent, false, browserComponent.findColour(AlertWindow::backgroundColourId));
  if (dialog.show()) {
    add_document(browserComponent.getSelectedFile(0));
  }
#endif
}
