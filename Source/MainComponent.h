#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "MenuModels.h"

#include <list>

class MainContentComponent   : public Component, public MainMenuModel::Listener, public ChangeListener
{
public:
  MainContentComponent();
  ~MainContentComponent();

  void paint (Graphics&) override;
  void resized() override;
  void menuItemSelected(int menuItemID) override;
  void changeListenerCallback(ChangeBroadcaster *source) override;

  void do_fileopen();
  void do_fileclose();
  void add_document(const File& file);

private:
  struct OpenDocument {
    OpenDocument(const File& f) : file(f), tokenizer(nullptr) {
      code_document = new CodeDocument;
      editor = new CodeEditorComponent(*code_document, tokenizer);
      String contents = f.loadFileAsString();
      code_document->replaceAllContent(contents);
      editor->setTabSize(2, true);
    }
    ~OpenDocument() {
      delete editor;
      delete code_document;
      delete tokenizer;
    }

    File file;
    CodeTokeniser* tokenizer;
    CodeEditorComponent* editor;
    CodeDocument* code_document;
  };
  
  typedef std::list<OpenDocument*> OpenDocsList;
  OpenDocsList _opendocs;
  

  ScopedPointer<Font> _editorFont;
  
  MenuBarComponent _menu;
  ScopedPointer<MainMenuModel> _model;
  
  TabbedComponent _tabs;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


