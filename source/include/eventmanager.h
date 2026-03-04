#pragma once
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "event/event.h"


class EventManager{
public:
  EventManager();

  /** makes the callback and registers the event */
  template<typename EventType> void registerEvent(std::function<void(const EventType&)> callback) {

    listeners[typeid(EventType)].push_back(
      [callback](const Event& e) {
        callback(static_cast<const EventType&>(e));
      }
    );

  }

  template<typename EventType> void activateEvent(const EventType& event) {
    auto it = listeners.find(typeid(EventType));
    if(it != listeners.end()) {
      for(auto &listener : it->second) {
        listener(event);
      }
    }
  }

private:
  std::unordered_map<std::type_index, std::vector<std::function<void(const Event&)>>> listeners;

};

EventManager& getEventManager();
