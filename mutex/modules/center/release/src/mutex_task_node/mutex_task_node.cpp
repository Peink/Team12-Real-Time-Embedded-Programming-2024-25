#include "mutex_task_node/mutex_task_node.hpp"

#include "mutex_task_node/mutex_task_impl.hpp"

namespace lcy {
mutexTaskNode::mutexTaskNode(const std::string& name)
    : impl_(std::make_unique<mutexTaskImpl>(this)) {}
mutexTaskNode::~mutexTaskNode() = default;

void mutexTaskNode::onInit() { impl_->onInit(); }

void mutexTaskNode::run(void* param) {
  while (getThreadRunFlag()) {
    impl_->run(param);
  }
}
}  // namespace lcy