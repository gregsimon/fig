
#pragma once

#include "MainWindow.h"

class MainMenuModel : public MenuBarModel {
public:
  MainMenuModel() {
    auto& commandManager = MainWindow::getApplicationCommandManager();

    file_menu.addCommandItem(&commandManager, MainWindow::FILE_New);
    file_menu.addCommandItem(&commandManager, MainWindow::FILE_Open);
    file_menu.addCommandItem(&commandManager, MainWindow::FILE_Close);
    file_menu.addSeparator();
    file_menu.addCommandItem(&commandManager, MainWindow::FILE_Save);
    file_menu.addCommandItem(&commandManager, MainWindow::FILE_SaveAs);
#if !defined(JUCE_MAC)
    file_menu.addSeparator();
    file_menu.addItem(StandardApplicationCommandIDs::quit, "Exit");
#endif
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

  virtual void 	menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override {
  }
  
private:
  PopupMenu file_menu;

};