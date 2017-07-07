

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class CustomFileBrowser    : public Component,
  public FileBrowserListener, 
  public Button::Listener
{
public:
  enum { open, save };

  CustomFileBrowser(const String& lastFolder, int type=open);
  ~CustomFileBrowser();


  class Listener {
    public:
        virtual ~Listener() {}
        virtual void fileSelected(const File&) = 0;
        virtual void cancelled() = 0;
  };
  void addListener(Listener* newListener) {
    _listeners.add(newListener);
  }
  void removeListener(Listener* newListener) {
    _listeners.remove(newListener);
  }


  void paint (Graphics&) override;
  void resized() override;

  // FileBrowserListener
  virtual void  fileClicked (const File &file, const MouseEvent &e) override;
  virtual void  selectionChanged () override { }
  virtual void  fileDoubleClicked (const File &file) override;
  virtual void  browserRootChanged (const File &newRoot) override { }

  // ButtonListener
  virtual void  buttonClicked (Button *) override;

private:
    ScopedPointer<FileBrowserComponent> _browser;
    TextButton _okButton;
    TextButton _cancelButton;

    WildcardFileFilter _filter;
    ListenerList<Listener> _listeners;

    CustomFileBrowser();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomFileBrowser)
};
