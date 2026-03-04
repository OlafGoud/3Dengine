#include "eventmanager.h"

EventManager eventManager;

EventManager& getEventManager() {
  return eventManager;
}

/**
 * registerevent() -> add pointer to function
 * callevent() -> run all functions that are part of the event

 * where store the pointers? event class or ?
 * event class ?
 * 
 */

EventManager::EventManager() {

}
