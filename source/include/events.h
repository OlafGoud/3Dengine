#include <string>
#include <vector>


class EventManager{
public:
  EventManager();

  void setEvent(std::string event);
  bool isEvent(std::string event);
  void removeEvent(std::string event);
  bool isEventAndRemove(std::string event);

private:

  std::vector<std::string> events;

};