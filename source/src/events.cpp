#include "events.h"
#include <algorithm>
#include <string>
#include "iostream"

EventManager::EventManager() {

}


void EventManager::setEvent(std::string event) {
  
  auto it = std::find(this->events.begin(), this->events.end(), event);
  
  if(it != this->events.end()) {
    return;
  }
  std::cout << "asdf\n";
  this->events.push_back(event);
}


bool EventManager::isEvent(std::string event) {

  auto it = std::find(this->events.begin(), this->events.end(), event);

  if(it == this->events.end()) {
    return false;
  }

  return true;
  
}

bool EventManager::isEventAndRemove(std::string event) {

  auto it = std::find(this->events.begin(), this->events.end(), event);

  if(it == this->events.end()) {
    return false;
  }

  events.erase(it);

  return true;
  
}

void EventManager::removeEvent(std::string event) {

  auto it = std::find(this->events.begin(), this->events.end(), event);

  if(it == this->events.end()) {
    return;
  }

  events.erase(it);

  return;

}