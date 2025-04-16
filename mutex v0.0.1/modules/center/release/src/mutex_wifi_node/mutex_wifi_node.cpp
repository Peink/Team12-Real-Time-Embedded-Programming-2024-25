#include "mutex_wifi_node/mutex_wifi_node.hpp"

#include "mutex_wifi_node/mutex_wifi_impl.hpp"

namespace lcy {
mutexWifiNode::mutexWifiNode(const std::string& name)
    : impl_(std::make_unique<mutexWifiImpl>(this)) {}
mutexWifiNode::~mutexWifiNode() = default;

void mutexWifiNode::onInit() { impl_->onInit(); }

void mutexWifiNode::run(void* param) {
  while (getThreadRunFlag()) {
    impl_->run(param);
  }
}
}  // namespace lcy