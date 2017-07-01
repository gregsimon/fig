#include "MainComponent.h"




MainContentComponent::MainContentComponent()
: _tabs( TabbedButtonBar::Orientation::TabsAtTop)
{
  // load the font
  int dataSize = 0;
  const char* data = BinaryData::getNamedResource("InconsolataRegular_ttf", dataSize);
  Typeface::Ptr font_data = Typeface::createSystemTypefaceFor(data, dataSize);
  _editorFont = new Font(font_data);
  _editorFont->setHeight(20.0f);

  // set up commands
  // TODO
  
  // setup menus
  _model = new MainMenuModel();
  _model->addListener(this);
#if JUCE_MAC
    MenuBarModel::setMacMainMenu(_model);
#endif
  _menu.setModel(_model);
  addAndMakeVisible(&_menu);
  
  // tabs
  addAndMakeVisible(&_tabs);


  setSize(1024, 768);
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
}

void MainContentComponent::paint (Graphics& )
{
}

void MainContentComponent::resized()
{
  _menu.setBounds(0, 0, getWidth(), 24);
  _tabs.setBounds(0, 24, getWidth(), getHeight()-48);
}

void MainContentComponent::menuItemSelected(int menuItemID)
{
  switch (menuItemID) {
  case FILE_Open: 
    do_fileopen();
    break;
  case FILE_Close:
    do_fileclose();
    break;
  case FILE_Exit:
      _menu.setModel(nullptr);
    JUCEApplication::getInstance()->perform(ApplicationCommandTarget::InvocationInfo(StandardApplicationCommandIDs::quit));
    break;
  }
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
