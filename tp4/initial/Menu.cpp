#include "Menu.hpp"

  MenuItem::MenuItem(int id, String line1, String line2){
    this->id = id;
    this->line1 = line1;
    this->line2 = line2;
    next = NULL;
    prev = NULL;
    enter = NULL;
    back = NULL;
  }
  MenuItem::~MenuItem(){
    if(next != NULL){
      delete next;
    }
    if(prev != NULL && prev != next){
      delete prev;
    }
    if(enter != NULL && enter != prev && enter != next){
      delete enter;
    }
    if(back != NULL && back != next && back != prev && back != enter){
      delete back;
    }
  }

  void MenuItem::setNavigation(MenuItem* next, MenuItem* prev, MenuItem* enter, MenuItem* back){
    this->next = next;
    this->prev = prev;
    this->enter = enter;
    this->back = back;
  }
  int MenuItem::getId(){
    return id;
  }
  String MenuItem::getLine1(){
    return line1;
  }
  String MenuItem::getLine2(){
    return line2;
  }
  MenuItem* MenuItem::getNext(){
    return next;
  }
  MenuItem* MenuItem::getPrev(){
    return prev;
  }
  MenuItem* MenuItem::getBack(){
    return back;
  }
  MenuItem* MenuItem::getEnter(){
    return enter;
  }

  Menu::Menu(LiquidCrystal* lcd){
    this->lcd = lcd;
    this->current = NULL;
  }
  Menu::~Menu(){
    if(current != NULL){
      delete current;
    }
  }

  void Menu::boot(){
    //INIT MENU OPTIONS
    MenuItem* menu0 = new MenuItem(0, "Equipe I.S.I.W.Y", "Press para comecar");
    MenuItem* menu1 = new MenuItem(1, "Caminhos", "");
    MenuItem* menu11 = new MenuItem(11, "Quadrado", "");
    MenuItem* menu12 = new MenuItem(12, "Retangulo", "");
    MenuItem* menu13 = new MenuItem(3, "Reta", "");

    menu0->setNavigation(menu1, menu1, menu1, menu1);
    menu1->setNavigation(NULL, NULL, menu11, menu0);
    menu11->setNavigation(menu12, menu13, NULL, menu1);
    menu12->setNavigation(menu13, menu11, NULL, menu1);
    menu13->setNavigation(menu11, menu12, NULL, menu1);


    current = menu0;
    
  }
  void Menu::display(){
    lcd->setCursor(0, 0);
    lcd->print(current->getLine1());
    lcd->print("                 ");

    lcd->setCursor(0, 1);
    lcd->print(current->getLine2());
    lcd->print("                 ");
  }
  void Menu::next(){
    MenuItem* i = current->getNext();
    if(i != NULL){
      current = i;
    }
  }
  void Menu::prev(){
    MenuItem* i = current->getPrev();
    if(i != NULL){
      current = i;
    }
  }
  void Menu::enter(){
    MenuItem* i = current->getEnter();
    if(i != NULL){
      current = i;
    }else{
      run(current);
    }
  }
  void Menu::back(){
    MenuItem* i = current->getBack();
    if(i != NULL){
      current = i;
    }
  }
  void Menu::run(MenuItem* item){
    int id = item->getId();
  }

