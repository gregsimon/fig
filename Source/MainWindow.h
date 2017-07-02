

#pragma once

class MainComponent;

class EditorLookAndFeel : public LookAndFeel_V4 {
public:
  EditorLookAndFeel() {
    int dataSize = 0;
    const char* data = BinaryData::getNamedResource("InconsolataRegular_ttf", dataSize);
    font = new Font(Typeface::createSystemTypefaceFor(data, dataSize));
    font->setHeight(16.0f);
  }
  Font getTextButtonFont(TextButton &, int ) override { return *font; }
  Font getMenuBarFont(MenuBarComponent &, int , const String &) override { return *font; }
  Font getPopupMenuFont() override { return *font; }
  Font getLabelFont(Label &) override { return *font; }

  void 	drawTabButton(TabBarButton &b, Graphics &g, bool , bool ) override {
    //LookAndFeel_V3::drawTabButton(b, g, isMouseOver, isMouseDown);
    TabbedButtonBar &bar = b.getTabbedButtonBar();
    StringArray arr = bar.getTabNames();
    String title = arr[b.getIndex()];
    bool is_selected = bar.getCurrentTabIndex() == b.getIndex();

    const Colour background_color(39, 40, 34);

    if (is_selected) {
      g.setFillType(FillType(background_color));
      g.fillRect(b.getTextArea());
      g.setColour(Colour(242, 242, 242));
    }
    else {
      ColourGradient grad(Colour(80,81,78), 0.0f, 0.0f, 
          Colour(49,48,47), 0.0f, (float)b.getTextArea().getBottom(), false);
      g.setGradientFill(grad);
      g.fillRect(b.getTextArea());
      g.setColour(Colour(152, 193, 170));
    }
    g.drawText(title,
      b.getTextArea(),
      Justification::centred, false);
  }
private:
  ScopedPointer<Font> font;
};

class MainWindow : 
  public DocumentWindow, 
  private AsyncUpdater
{
public:
  MainWindow(String name);
  ~MainWindow();

  static ApplicationCommandManager& getApplicationCommandManager();

  enum CommandIDs {
    FILE_New = 0x2000,
    FILE_Open,
    FILE_Close,
    FILE_Save,
    FILE_SaveAs,

    EDIT_Undo,
    EDIT_Redo,
    EDIT_Cut,
    EDIT_Copy,
    EDIT_Paste,
    EDIT_SelectAll,


    FILE_Exit
  };

  void closeButtonPressed() override;

  /* Note: Be careful if you override any DocumentWindow methods - the base
  class uses a lot of them, so by overriding you might break its functionality.
  It's best to do all your work in your content component instead, but if
  you really have to override any DocumentWindow methods, make sure your
  subclass also calls the superclass's method.
  */

private:
  ScopedPointer<MainComponent> mainComponent;
  EditorLookAndFeel darkLookAndFeel;

  void handleAsyncUpdate() override;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
