#include "mutex_data_node/mutex_data_impl.hpp"

#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace lcy {

mutexDataNode::mutexDataImpl::mutexDataImpl() {}

mutexDataNode::mutexDataImpl::mutexDataImpl(mutexNode *node) : node_(node) {}

void mutexDataNode::mutexDataImpl::onInit() {
  std::cout << "create mutex data success\r\n" << std::endl;
  mutexSerialImpl_ = std::make_shared<mutexSerialImpl>(node_);
  mutexSerialImpl_->onInit();
}

void mutexDataNode::mutexDataImpl::run(void *param) {
  auto ev = node_->getEvent();
  if (ev) {
    switch (ev->getId()) {
      default:
        break;
    }
  }
}

}  // namespace lcy