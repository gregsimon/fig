
#pragma once

enum MenuIds {
  FILE_New = 10,
  FILE_Open,
  FILE_Save,
  FILE_SaveAs,
  FILE_Exit
};

class MainMenuModel : public MenuBarModel {
public:
  MainMenuModel() {}

  class Listener {
  public:
    virtual ~Listener() {}
    virtual void menuItemSelected(int menuItemID) = 0;
  };

  void addListener(Listener* newListener) {
    _listeners.add(newListener);
  }
  void removeListener(Listener* newListener) {
    _listeners.remove(newListener);
  }

  virtual StringArray getMenuBarNames() override {
    StringArray arr;
    arr.add("File");
    return arr;
  }
  virtual PopupMenu getMenuForIndex(int topLevelMenuIndex, const String &menuName) override {
    PopupMenu m;
    switch (topLevelMenuIndex) {
    case 0:
      // File
      m.addItem(FILE_New, "New...", true);
      m.addItem(FILE_Open, "Open...", true);
      m.addSeparator();
      m.addItem(FILE_Save, "Save", true);
      m.addItem(FILE_SaveAs, "Save As...", true);
      m.addSeparator();
      m.addItem(FILE_Exit, "Exit", true);
      break;

    }

    return m;
  }
  virtual void 	menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/) override {
    _listeners.call(&Listener::menuItemSelected, menuItemID);
  }
private:
  ListenerList<Listener> _listeners;
};
