#ifndef _MUTEX_NODE_H_
#define _MUTEX_NODE_H_

#include <error.h>
#include <gpiod.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#define HOME_PREFIX "/home/team12/embedded-project-door-lock/"

typedef struct {
  unsigned int line_num;
  struct gpiod_line* line;
  int val;
} gpio_t;

enum {
  NULL_EVENT,
  KEY_PRESS_EVENT,
  ECHO_DISTANCE_DANGER_EVENT,
  ADD_FINGER_EVENT,
  VERIFICATION_FINGER_EVENT,
  DELETE_FINGER_EVENT,
};

namespace lcy {

class mutex_event_with_param {
 public:
  mutex_event_with_param() {
    event_type = NULL_EVENT;
    param = nullptr;
  }
  ~mutex_event_with_param() = default;

  int event_type;
  void* param;
};

class mutexNode {
 public:
  explicit mutexNode() = default;
  ~mutexNode() = default;

  virtual void onInit() = 0;

  void setNodeCallbackInterfaceList(
      std::vector<std::shared_ptr<mutexNode>>* list) {
    nodeCallbackInterfaceList = list;
  }

  void eventLoop(mutex_event_with_param& e) {
    if (nodeCallbackInterfaceList != nullptr) {
      for (int i = 0; i < nodeCallbackInterfaceList->size(); i++) {
        nodeCallbackInterfaceList->at(i)->hasEventCallback(e);
      }
    }
  }

  virtual void hasEventCallback(mutex_event_with_param& e) = 0;

  std::vector<std::shared_ptr<mutexNode>>* nodeCallbackInterfaceList = nullptr;

  char play_order[2048];

  void playVideo(const char* file) {
    memset(play_order, 0, 2048);
    snprintf(play_order, 2048, "aplay %s%s%s", HOME_PREFIX, "audio/",file);
    int result = system(play_order);
    if (result == 0) {
      // std::cout << "Script executed successfully." << std::endl;
    } else {
      std::cerr << "Script execution failed with code: " << result << std::endl;
    }
  }

 private:
};
}  // namespace lcy

#endif