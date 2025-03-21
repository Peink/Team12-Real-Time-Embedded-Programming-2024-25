#ifndef _MUTEX_TASK_IMPL_H_
#define _MUTEX_TASK_IMPL_H_

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>

#include "mutex_common.hpp"
#include "mutex_task_node/mutex_task_node.hpp"

namespace lcy {
class mutexTaskNode::mutexTaskImpl {
 public:
  mutexTaskImpl();
  mutexTaskImpl(mutexNode* node);

  void onInit();
  void run(void* param);

 private:
  mutexNode* node_;

  void receiveFingerPrintDataCallback(lcy::mutex_cb_param_t* param);
};
}  // namespace lcy

#endif