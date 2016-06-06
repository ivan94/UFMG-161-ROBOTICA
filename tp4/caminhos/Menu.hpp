#ifndef __MENU_H
#define __MENU_H
#include <LiquidCrystal.h>

class MenuItem{
private:
  int id;
  String line1, line2;
  MenuItem *next, *prev, *enter, *back;
public:
  MenuItem(int id, String line1, String line2);
  ~MenuItem();

  void setNavigation(MenuItem* next, MenuItem* prev, MenuItem* enter, MenuItem* back);
  int getId();
  String getLine1();
  String getLine2();
  MenuItem* getNext();
  MenuItem* getPrev();
  MenuItem* getBack();
  MenuItem* getEnter();
};

class Menu{
private:
  LiquidCrystal* lcd;
  MenuItem* current;
public:
  Menu(LiquidCrystal* lcd);
  ~Menu();

  void boot();
  void display();
  void next();
  void prev();
  void enter();
  void back();
  void run(MenuItem* item);
};

#endif
