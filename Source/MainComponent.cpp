#include "MainComponent.h"




MainContentComponent::MainContentComponent()  
{
  _codeDocument = new CodeDocument();
  _editor = new CodeEditorComponent(*_codeDocument, nullptr);
  addAndMakeVisible(_editor);

  _model = new MainMenuModel();
  _model->addListener(this);
  _menu.setModel(_model);
  addAndMakeVisible(&_menu);

  setSize(1024, 768);
}

MainContentComponent::~MainContentComponent()
{
  _menu.setModel(nullptr);
  
  // order here matters:
  delete _editor;
  delete _codeDocument;
}

void MainContentComponent::paint (Graphics& g)
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
  _editor->setBounds(0, 24, getWidth(), getHeight());
}

void MainContentComponent::menuItemSelected(int menuItemID)
{
  switch (menuItemID) {
  case FILE_Open: 
    do_fileopen();
    break;
  case FILE_Exit:
    JUCEApplication::getInstance()->perform(ApplicationCommandTarget::InvocationInfo(StandardApplicationCommandIDs::quit));
    break;
  }
}

void MainContentComponent::do_fileopen() {

  int flags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
  WildcardFileFilter wildcard("*.*", "*", String());
  String startingFile;
  
  FileBrowserComponent browserComponent(flags, startingFile, &wildcard, nullptr);

  FileChooserDialogBox dialog("Open", String(),
    browserComponent, false, browserComponent.findColour(AlertWindow::backgroundColourId));
  if (dialog.show()) {

  }
}