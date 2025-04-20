#ifndef _MUTEX_NODE_H_
#define _MUTEX_NODE_H_

#include <error.h>
#include <gpiod.h>

#include <memory>
#include <string>
#include <vector>

typedef struct {
  unsigned int line_num;
  struct gpiod_line* line;
  int val;
} gpio_t;

enum {
  NULL_EVENT,
  KEY_PRESS_EVENT,
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

 private:
};
}  // namespace lcy

#endif