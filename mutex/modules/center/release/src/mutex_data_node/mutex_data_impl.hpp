#ifndef _MUTEX_DATA_IMPL_H_
#define _MUTEX_DATA_IMPL_H_

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>

#include "data_serial_impl.hpp"
#include "mutex_common.hpp"
#include "mutex_data_node/mutex_data_node.hpp"

namespace lcy {
class mutexDataNode::mutexDataImpl {
 public:
  mutexDataImpl();
  mutexDataImpl(mutexNode* node);

  void onInit();
  void run(void* param);

 private:
  mutexNode* node_;
  std::shared_ptr<mutexSerialImpl> mutexSerialImpl_;
};
}  // namespace lcy

#endif