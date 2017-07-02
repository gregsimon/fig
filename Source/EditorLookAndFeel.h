
#pragma once

// --------------------------------------------------------------------------------

class EditorLookAndFeel : public LookAndFeel_V4 {
public:
  EditorLookAndFeel() {
    int dataSize = 0;
    const char* data = BinaryData::getNamedResource("InconsolataRegular_ttf", dataSize);
    font = new Font(Typeface::createSystemTypefaceFor(data, dataSize));
    font->setHeight(16.0f);

    setColour(0x1004500, Colour(39, 40, 34)); // JUCE\modules\juce_gui_extra\code_editor\juce_CodeEditorComponent.h
    setColour(0x1004502, Colour(73, 72, 62)); // highlight color
    setColour(0x1004505, Colour(114, 144, 138)); // lineNumberTextId
  }
  Font getTextButtonFont(TextButton &, int) override { return *font; }
  Font getMenuBarFont(MenuBarComponent &, int, const String &) override { return *font; }
  Font getPopupMenuFont() override { return *font; }
  Font getLabelFont(Label &) override { return *font; }

  void 	drawTabButton(TabBarButton &b, Graphics &g, bool, bool) override {
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
      ColourGradient grad(Colour(80, 81, 78), 0.0f, 0.0f,
        Colour(49, 48, 47), 0.0f, (float)b.getTextArea().getBottom(), false);
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

