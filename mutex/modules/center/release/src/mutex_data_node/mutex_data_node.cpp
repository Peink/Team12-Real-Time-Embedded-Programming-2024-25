#include "mutex_data_node/mutex_data_node.hpp"

#include "mutex_data_node/mutex_data_impl.hpp"

namespace lcy {
mutexDataNode::mutexDataNode(const std::string& name)
    : impl_(std::make_unique<mutexDataImpl>(this)) {}
mutexDataNode::~mutexDataNode() = default;

void mutexDataNode::onInit() { impl_->onInit(); }

void mutexDataNode::run(void* param) {
  while (getThreadRunFlag()) {
    impl_->run(param);
  }
}
}  // namespace lcy