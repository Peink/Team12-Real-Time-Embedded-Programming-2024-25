#include "mutex_task_node/mutex_task_impl.hpp"

#include <fstream>
#include <iostream>

namespace lcy {
mutexTaskNode::mutexTaskImpl::mutexTaskImpl() {}

mutexTaskNode::mutexTaskImpl::mutexTaskImpl(mutexNode *node) : node_(node) {}

void mutexTaskNode::mutexTaskImpl::onInit() {
  std::cout << "create mutex task success\r\n" << std::endl;
  node_->subscribe(EventId::DATA_RECEIVE_FINGER_DATA_EVENT);
}

void mutexTaskNode::mutexTaskImpl::run(void *param) {
  auto ev = node_->getEvent();
  if (ev) {
    switch (ev->getId()) {
      case EventId::DATA_RECEIVE_FINGER_DATA_EVENT: {
        // char* data = (char *)ev->getData<>();
        lcy::mutex_cb_param_t *param =
            (lcy::mutex_cb_param_t *)ev->getData<lcy::mutex_cb_param_t>();
        receiveFingerPrintDataCallback(param);
      } break;
      default:
        break;
    }
  }
}

void mutexTaskNode::mutexTaskImpl::receiveFingerPrintDataCallback(
    lcy::mutex_cb_param_t *param) {
  printf("receive finger print data : %s\r\n", param->data);
  __DELETE_MUTEX_CB_PARAM_T_(param);
}

}  // namespace lcy