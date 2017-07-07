
#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomFileBrowser.h"

CustomFileBrowser::CustomFileBrowser(const String& lastFolder, int type)
 : _filter("*.cpp;*.h;*.cc;*.hpp;*.c;*.dart;*.lua;*.txt;*.gn;*.gni;", "*", String())
{
  int dialog_flags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

  _browser = new FileBrowserComponent(dialog_flags, lastFolder, &_filter, nullptr);
  _browser->addListener (this);
  addAndMakeVisible(_browser);

  if (type == open)
    _okButton.setButtonText("Open");
  else if (type == save)
    _okButton.setButtonText("Save");
  _okButton.addListener(this);
  addAndMakeVisible(&_okButton);

  _cancelButton.setButtonText("Cancel");
  _cancelButton.addListener(this);
  addAndMakeVisible(&_cancelButton);
}

CustomFileBrowser::~CustomFileBrowser()
{
  _browser->removeListener(this);
  _cancelButton.removeListener(this);
}

void CustomFileBrowser::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void CustomFileBrowser::resized()
{
  const int kButtonHeight = 32;
  const int kButtonWidth = 64;
  const int kMargin = 12;

  const int w = getWidth();
  const int h = getHeight();

  if (_browser)
    _browser->setBounds(0, 0, w, h-(2*kMargin)-kButtonHeight);

  _cancelButton.setBounds(w-kMargin-kButtonWidth, h-kMargin-kButtonHeight,
      kButtonWidth, kButtonHeight);
  _okButton.setBounds(w-(2*(kMargin+kButtonWidth)), h-kMargin-kButtonHeight,
      kButtonWidth, kButtonHeight);

}

void CustomFileBrowser::fileClicked (const File &file, const MouseEvent &)
{
}

void CustomFileBrowser::fileDoubleClicked (const File &file)
{
  _listeners.call(&Listener::fileSelected, file);
}

void CustomFileBrowser::buttonClicked (Button *button)
{
  if (button == &_cancelButton)
    _listeners.call(&Listener::cancelled);
  else if (button == &_okButton) {
    _listeners.call(&Listener::fileSelected, _browser->getSelectedFile(0));
  }
}