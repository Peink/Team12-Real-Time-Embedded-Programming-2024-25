#ifndef _NODE_MUTEX_IMPL_H_
#define _NODE_MUTEX_IMPL_H_

#include <thread>
#include <vector>

#include "mutex_loader.hpp"

class NodeImpl {
 public:
  NodeImpl();
  ~NodeImpl();

  void start();

 private:
  const std::unique_ptr<lcy::mutexLoader> loader_;
};

#endif