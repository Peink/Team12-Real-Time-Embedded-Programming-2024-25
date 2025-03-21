#include "mutex_wifi_node/mutex_wifi_impl.hpp"

#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace lcy {

static std::string wifiStatus_ = "DISCONNECTED";
static std::string mqttStatus_ = "DISCONNECTED";
mutexWifiNode::mutexWifiImpl::mutexWifiImpl() {}

mutexWifiNode::mutexWifiImpl::mutexWifiImpl(mutexNode *node) : node_(node) {}

void mutexWifiNode::mutexWifiImpl::onInit() {
  std::cout << "create mutex wifi success\r\n" << std::endl;
}

void mutexWifiNode::mutexWifiImpl::run(void *param) {
  auto ev = node_->getEvent();
  if (ev) {
    switch (ev->getId()) {
      default:
        break;
    }
  }
}

}  // namespace lcy