#ifndef _NODE_MUTEX_IMPL_H_
#define _NODE_MUTEX_IMPL_H_

#include <sys/epoll.h>

#include <thread>
#include <vector>

#include "mutex_common.hpp"
#include "mutex_loader.hpp"

class NodeImpl {
 public:
  NodeImpl();
  ~NodeImpl();

  void onInit(void* param);
  void start(void* param);

 private:
  const std::unique_ptr<lcy::mutexLoader> loader_;
  std::unique_ptr<std::thread> thread_;

  void run_(void* param);

  int node_num_ = 0;
  bool mutexImplThreadRunFlag_ = true;
};

#endif