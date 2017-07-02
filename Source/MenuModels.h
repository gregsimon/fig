
#pragma once

enum MenuIds {
  FILE_New = 10,
  FILE_Open,
  FILE_Close,
  FILE_Save,
  FILE_SaveAs
};

class MainMenuModel : public MenuBarModel {
public:
  MainMenuModel(ApplicationCommandManager* cmdmgr) {
    file_menu.addCommandItem(cmdmgr, FILE_New);
    file_menu.addCommandItem(cmdmgr, FILE_Open);
    file_menu.addCommandItem(cmdmgr, FILE_Close);
    file_menu.addSeparator();
    file_menu.addCommandItem(cmdmgr, FILE_Save);
    file_menu.addCommandItem(cmdmgr, FILE_SaveAs);
#if !defined(JUCE_MAC)
    file_menu.addSeparator();
    file_menu.addItem(StandardApplicationCommandIDs::quit, "Exit");
#endif
  }

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
  virtual PopupMenu getMenuForIndex(int topLevelMenuIndex, const String &) override {
    PopupMenu m;
    switch (topLevelMenuIndex) {
    case 0:
      return file_menu;
      break;

    }

    return m;
  }
  virtual void 	menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/) override {
    _listeners.call(&Listener::menuItemSelected, menuItemID);
  }
private:
  ListenerList<Listener> _listeners;
  PopupMenu file_menu;

  MainMenuModel() {}
};
