#ifndef _MUTEX_WIFI_IMPL_H_
#define _MUTEX_WIFI_IMPL_H_

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>

#include "mutex_common.hpp"
#include "mutex_wifi_node/mutex_wifi_node.hpp"
#include "periodTask.hpp"

namespace lcy {
class mutexWifiNode::mutexWifiImpl {
 public:
  mutexWifiImpl();
  mutexWifiImpl(mutexNode* node);

  void onInit();
  void run(void* param);

 private:
  mutexNode* node_;
};
}  // namespace lcy

#endif