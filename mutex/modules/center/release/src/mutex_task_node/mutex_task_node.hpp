#ifndef _MUTEX_TASK_NODE_H_
#define _MUTEX_TASK_NODE_H_

#include "mutex_node.hpp"

namespace lcy {
class mutexTaskNode : public mutexNode {
 public:
  explicit mutexTaskNode(const std::string& name);
  ~mutexTaskNode() override;

  void onInit() override;
  void run(void* param) override;

 private:
  class mutexTaskImpl;
  std::unique_ptr<mutexTaskImpl> impl_;
};
}  // namespace lcy

#endif