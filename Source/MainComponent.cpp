
#include "../JuceLibraryCode/JuceHeader.h"
#include "MainWindow.h"
#include "MainComponent.h"

const int kTabHeaderHeight = 22;
const int kFindPanelHeight = 24;

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

  _editorFontSize = (float)_settings->getDoubleValue("editor_font_size", 16.5);

  int dataSize = 0;
  const char* data = BinaryData::getNamedResource("InconsolataRegular_ttf", dataSize);
  Typeface::Ptr font_data = Typeface::createSystemTypefaceFor(data, dataSize);
  _editorFont = new Font(font_data);
  _editorFont->setHeight(_editorFontSize);

  // find panel
  _findPanel = new FindAndReplaceComponent;
  addAndMakeVisible(_findPanel);
  _findPanel->addListener(this);
  
  // set up tabs
  _tabs.setTabBarDepth(kTabHeaderHeight);

  // set up commands
  // TODO

  // menu bar
  _menu = new MenuBarComponent(this);
  setApplicationCommandManagerToWatch(&MainWindow::getApplicationCommandManager());
#if JUCE_MAC
  MenuBarModel::setMacMainMenu(this);
#else
  addAndMakeVisible(_menu);
#endif
  
  // open document tabs
  addAndMakeVisible(&_tabs);

  // restore window size from settings
  setSize(_settings->getIntValue("win_width", 1024), _settings->getIntValue("win_height", 768));
  
  // restore any tabs that were opened when we last closed.
  String last_opened_files = _settings->getValue("last_opened_files");
  String file_name = last_opened_files.upToFirstOccurrenceOf("|", false, false);
  add_document(File(file_name));
  while (1) {
    last_opened_files = last_opened_files.substring(file_name.length()+1);
    file_name = last_opened_files.upToFirstOccurrenceOf("|", false, false);
    if (!file_name.length())
      break;
    
    add_document(File(file_name));
  }
}

MainComponent::~MainComponent()
{
  setApplicationCommandManagerToWatch(nullptr);

  save_open_file_positions();
#if JUCE_MAC
  MenuBarModel::setMacMainMenu(nullptr);
#endif
  PopupMenu::dismissAllActiveMenus();
  _menu->setModel(nullptr);

  for (std::list<OpenDocument*>::iterator it = _opendocs.begin(); it != _opendocs.end(); ++it) {
    delete (*it);
  }

}


void MainComponent::paint (Graphics& )
{
}

void MainComponent::resized()
{
#if JUCE_MAC
  const int kMenuHeight = 0;
#else
  const int kMenuHeight = 24;
#endif

  int find_panel_height = (_findPanel->isVisible() ? kFindPanelHeight : 0);

  _menu->setBounds(0, 0, getWidth(), kMenuHeight);
  _tabs.setBounds(0, kMenuHeight, getWidth(), getHeight()-kMenuHeight-find_panel_height);

  if (_findPanel->isVisible()) {
    _findPanel->setBounds(0, _tabs.getHeight() + kMenuHeight,
      getWidth(), find_panel_height);
  }
  
  if (_fileBrowser) {
    _fileBrowser->setBounds(0, 0, getWidth(), getHeight());
  }
  
  // save the outer window size.
  _settings->setValue("win_width", getWidth());
  _settings->setValue("win_height", getHeight());
}

StringArray MainComponent::getMenuBarNames() {
  StringArray arr;
  arr.add("File");
  arr.add("Edit");
  arr.add("View");
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
    m.addCommandItem(commandManager, MainWindow::FILE_Recent1);
    m.addCommandItem(commandManager, MainWindow::FILE_Recent2);
    m.addCommandItem(commandManager, MainWindow::FILE_Recent3);
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
    m.addSeparator();
    m.addCommandItem(commandManager, MainWindow::EDIT_Find);
    break;
  case 2:
    m.addCommandItem(commandManager, MainWindow::VIEW_NextDoc);
    m.addCommandItem(commandManager, MainWindow::VIEW_PrevDoc);
    m.addCommandItem(commandManager, MainWindow::VIEW_TextLarger);
    m.addCommandItem(commandManager, MainWindow::VIEW_TextSmaller);
    break;
  }

  return m;
}

void MainComponent::menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) { }
void MainComponent::changeListenerCallback(ChangeBroadcaster *) { }

void MainComponent::do_find() {
  _findPanel->focus();
}

void MainComponent::returnKeyPressed(const String& findText) {
  Logger::outputDebugString(findText);
}

void MainComponent::show_find_ui(bool show) {

}

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

void MainComponent::show_next_tab()
{
  if (!_tabs.getNumTabs())
    return;

  int index = _tabs.getCurrentTabIndex();
  index++;
  if (index >= _tabs.getNumTabs())
    index = 0;

  _tabs.setCurrentTabIndex(index);
}

void MainComponent::show_prev_tab()
{
  if (!_tabs.getNumTabs())
    return;

  int index = _tabs.getCurrentTabIndex();
  index--;
  if (index < 0)
    index = _tabs.getNumTabs() - 1;

  _tabs.setCurrentTabIndex(index);
}

void MainComponent::save_open_file_positions()
{
  // save all the open documents' paths so we can re-open them when we start again.
  String saved_names;
  for (OpenDocsList::iterator it = _opendocs.begin(); it != _opendocs.end(); ++it) {
    OpenDocument* d = *it;
    saved_names << d->file.getFullPathName() << String("|");
  }
  _settings->setValue("last_opened_files", saved_names);
}

bool MainComponent::add_document(const File& file)
{
  if (!file.existsAsFile())
    return false;

  // Is this file already open? Then just select it.
  int index = 0;
  for (OpenDocsList::iterator it = _opendocs.begin(); it != _opendocs.end(); ++it, ++index) {
    if ((*it)->file == file) {
      _tabs.setCurrentTabIndex(index);
      return false;
    }
  }

  OpenDocument* doc = new OpenDocument;
  if (!doc->load(file)) {
    delete doc;
    return false;
  }

  _opendocs.push_back(doc);
  
  doc->editor->setFont(*_editorFont);
  _tabs.addTab(file.getFileName(), Colour(66, 67, 65), doc->editor, false);
  _tabs.setCurrentTabIndex(_tabs.getNumTabs() - 1);
  resized();

  return true;
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
                           File(_settings->getValue("last folder")),
                           "*");
    if (myChooser.browseForFileToOpen()) {
      file = myChooser.getResult();
    }
#else
  
  _fileBrowser = new CustomFileBrowser(_settings->getValue("last folder"), 
                CustomFileBrowser::open);
  _fileBrowser->addListener(this);
  addChildComponent(_fileBrowser, -1);
  _fileBrowser->setVisible(true);
  resized();
#endif
}

void MainComponent::fileSelected(const File& file)
{
  if (file.getFullPathName().length()) {
    if (add_document(file)) {
      _settings->setValue("last folder", file.getParentDirectory().getFullPathName());

      String name;
      name << "recent" << _recentCounter;
      _settings->setValue(name, file.getFullPathName());
      _recentCounter = (_recentCounter + 1) % 3;
    }
  }

  _fileBrowser->removeListener(this);
  removeChildComponent(_fileBrowser);
  delete (_fileBrowser.release());
}

void MainComponent::cancelled()
{
  _fileBrowser->removeListener(this);
  removeChildComponent(_fileBrowser);
  delete (_fileBrowser.release());
  resized();
}
