#ifndef _MUTEX_NODE_H_
#define _MUTEX_NODE_H_

#include <error.h>

#include <string>

namespace lcy {

class mutexNode {
 public:
  explicit mutexNode() = default;
  ~mutexNode() = default;

  virtual void onInit() = 0;

 private:
};
}  // namespace lcy

#endif