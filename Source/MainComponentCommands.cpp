#include "../JuceLibraryCode/JuceHeader.h"
#include "MainWindow.h"
#include "MainComponent.h"


ApplicationCommandTarget *  MainComponent::getNextCommandTarget()
{
  return findFirstTargetParentComponent();
}

void MainComponent::getAllCommands(Array< CommandID > &commands)
{
  const CommandID ids[] = { 
    MainWindow::FILE_Open,
    MainWindow::FILE_Close,
    MainWindow::FILE_Save,
    MainWindow::FILE_SaveAs,

    MainWindow::FILE_Recent1,
    MainWindow::FILE_Recent2,
    MainWindow::FILE_Recent3,

    MainWindow::EDIT_Undo,
    MainWindow::EDIT_Redo,
    MainWindow::EDIT_Cut,
    MainWindow::EDIT_Copy,
    MainWindow::EDIT_Paste,
    MainWindow::EDIT_SelectAll,
    MainWindow::EDIT_Find,

    MainWindow::VIEW_PrevDoc,
    MainWindow::VIEW_NextDoc,
    MainWindow::VIEW_TextLarger,
    MainWindow::VIEW_TextSmaller,

#if !defined(JUCE_MAC)
    MainWindow::FILE_Exit,
#endif
    
    MainWindow::FILE_New
    
  };

  commands.addArray(ids, numElementsInArray(ids));
}

void MainComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo &result)
{
  const String generalCategory("General");

  switch (commandID) {
  // File
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

  case MainWindow::FILE_Recent1:
    result.setInfo(_settings->getValue("recent0", String("-")), String(), generalCategory, 0);
    break;
  case MainWindow::FILE_Recent2:
    result.setInfo(_settings->getValue("recent1", String("-")), String(), generalCategory, 0);
    break;
  case MainWindow::FILE_Recent3:
    result.setInfo(_settings->getValue("recent2", String("-")), String(), generalCategory, 0);
    break;

  // Edit
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
  case MainWindow::EDIT_Find:
    result.setInfo("Find...", "find in file", generalCategory, 0);
    result.addDefaultKeypress('f', ModifierKeys::commandModifier);
    break;

  // View
  case MainWindow::VIEW_PrevDoc:
    result.setInfo("Focus Previous Doc", "", generalCategory, 0);
    result.addDefaultKeypress(KeyPress::tabKey, ModifierKeys::ctrlModifier|ModifierKeys::shiftModifier);
    break;
  case MainWindow::VIEW_NextDoc:
    result.setInfo("Focus Next Doc", "", generalCategory, 0);
    result.addDefaultKeypress(KeyPress::tabKey, ModifierKeys::ctrlModifier);
    break;
  case MainWindow::VIEW_TextLarger:
    result.setInfo("Make Text Larger", "", generalCategory, 0);
    result.addDefaultKeypress('=', ModifierKeys::ctrlModifier);
    break;
  case MainWindow::VIEW_TextSmaller:
    result.setInfo("Make Text Smaller", "", generalCategory, 0);
    result.addDefaultKeypress('-', ModifierKeys::ctrlModifier);
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
  case MainWindow::FILE_New:
    break;
  case MainWindow::FILE_Save:
    break;
  case MainWindow::VIEW_TextSmaller:
  {
    OpenDocument* doc = *_opendocs.begin();
    _editorFontSize -= .1f;
    _editorFont->setHeight(_editorFontSize);
    doc->editor->setFont(*_editorFont);
    doc->editor->repaint();
    Logger::outputDebugString(String::formatted("%f", _editorFontSize));
  }
  break;
  case MainWindow::VIEW_TextLarger:
    {
      OpenDocument* doc = * _opendocs.begin();
      _editorFontSize += .1f;
      _editorFont->setHeight(_editorFontSize);
      doc->editor->setFont(*_editorFont);
      doc->editor->repaint();
      Logger::outputDebugString(String::formatted("%f", _editorFontSize));
    }
    break;
  case MainWindow::FILE_Recent1:
    add_document(_settings->getValue("recent0"));
    break;
  case MainWindow::FILE_Recent2:
    add_document(_settings->getValue("recent1"));
    break;
  case MainWindow::FILE_Recent3:
    add_document(_settings->getValue("recent2"));
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
  case MainWindow::EDIT_Find:
    do_find();
    break;

  case MainWindow::VIEW_NextDoc:
    show_next_tab();
    break;
  case MainWindow::VIEW_PrevDoc:
    show_prev_tab();
    break;
  }
  return true;
}
