#include "MainComponent.h"




MainContentComponent::MainContentComponent()
: _tabs( TabbedButtonBar::Orientation::TabsAtTop)
{
  _codeDocument = new CodeDocument();
  _editor = new CodeEditorComponent(*_codeDocument, nullptr);
  _editor->setTabSize(2, true);
  addAndMakeVisible(_editor);

  // load the font
  int dataSize = 0;
  const char* data = BinaryData::getNamedResource("InconsolataRegular_ttf", dataSize);
  Typeface::Ptr font_data = Typeface::createSystemTypefaceFor(data, dataSize);
  _editorFont = new Font(font_data);
  _editorFont->setHeight(20.0f);
  _editor->setFont(*_editorFont);
  

  _model = new MainMenuModel();
  _model->addListener(this);
#if JUCE_MAC
    MenuBarModel::setMacMainMenu(_model);
#endif
  _menu.setModel(_model);
  addAndMakeVisible(&_menu);
  
  addAndMakeVisible(&_tabs);
  _tabs.addTab("main.cc", Colour(90, 90, 255), 0);
  //_tabs.addTab("foo.h", Colour(255, 0, 0), 0);

  setSize(1024, 768);
}

MainContentComponent::~MainContentComponent()
{
  _menu.setModel(nullptr);
#if JUCE_MAC
  MenuBarModel::setMacMainMenu(nullptr);
#endif
  
  // order here matters:
  delete _editor;
  delete _codeDocument;
}

void MainContentComponent::paint (Graphics& )
{
  /*
  g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

  g.setFont (Font (16.0f));
  g.setColour (Colours::white);
  g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
  */
}

void MainContentComponent::resized()
{
  _menu.setBounds(0, 0, getWidth(), 24);
  _tabs.setBounds(0, 24, getWidth(), 24);
  _editor->setBounds(0, 48, getWidth(), getHeight());
}

void MainContentComponent::menuItemSelected(int menuItemID)
{
  switch (menuItemID) {
  case FILE_Open: 
    do_fileopen();
    break;
  case FILE_Exit:
      _menu.setModel(nullptr);
    JUCEApplication::getInstance()->perform(ApplicationCommandTarget::InvocationInfo(StandardApplicationCommandIDs::quit));
    break;
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
    File file = browserComponent.getSelectedFile(0);
    String contents = file.loadFileAsString();
    _codeDocument->replaceAllContent(contents);
    _tabs.setTabName(0, file.getFileName());
  }
#endif
}
