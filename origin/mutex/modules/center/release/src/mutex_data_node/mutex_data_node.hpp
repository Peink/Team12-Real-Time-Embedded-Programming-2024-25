#ifndef _MUTEX_DATA_NODE_H_
#define _MUTEX_DATA_NODE_H_

#include "mutex_node.hpp"

namespace lcy {
class mutexDataNode : public mutexNode {
 public:
  explicit mutexDataNode(const std::string& name);
  ~mutexDataNode() override;

  void onInit() override;
  void run(void* param) override;

 private:
  class mutexDataImpl;
  std::unique_ptr<mutexDataImpl> impl_;
};
}  // namespace lcy

#endif