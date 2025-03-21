#include "mutex_data_node/data_serial_impl.hpp"

#include "mutex_common.hpp"
#include "serial/serialTermios.hpp"

mutexSerialImpl::~mutexSerialImpl() { serialDataCheckFlag_ = false; }

void mutexSerialImpl::onInit() {
  for (auto it : serialmp_) {
    serialList_.push_back(std::make_shared<SerialTermios>(
        it.second.port, it.second.baudrate, it.first));
    if (!serialList_.back()->serialOpen()) {
      printf("%s serial open failed \r\n", it.first.data());
    }
  }

  serialDataCheckThread_ = std::make_shared<std::thread>(
      &mutexSerialImpl::serialDataCheckRun_, this);
  serialDataCheckThread_->detach();
}

void mutexSerialImpl::serialDataCheckRun_() {
  while (serialDataCheckFlag_) {
    if (!serialList_.empty()) {
      for (int i = 0; i < serialList_.size(); i++) {
        if (serialList_[i]->isOpen_) {
          int readLen = serialList_[i]->receiveData(readBuf_, READ_BUF_LEN);
          if (readLen > 0) {
            lcy::mutex_cb_param_t* param = nullptr;
            __CREATE_MUTEX_CB_PARAM_T_(param, readBuf_, readLen);
            if (serialList_[i]->name_ == "fingerPrint") {
              static lcy::mutexEvent fingerPrintDataEvent;
              fingerPrintDataEvent.setId(
                  EventId::DATA_RECEIVE_FINGER_DATA_EVENT);
              fingerPrintDataEvent.setData(param);
              node_->publish(&fingerPrintDataEvent);
            }
          }
        }
      }
    }
    usleep(20 * 1000);
  }
}