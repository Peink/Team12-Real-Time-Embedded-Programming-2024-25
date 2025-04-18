#include "data_finger_impl.hpp"

#include "fingerlib/finger.hpp"
#include "serialTermios.hpp"

#define ADD_NUM 16
#define DELETE_NUM 15
#define VERIFICATION_NUM 14

static dataFingerImpl* this_ = nullptr;

static fingerEventInterFace fingerPrint_;

static void sendFingerPrintSerialData(const char* data, uint16_t data_len) {
  if (this_ != nullptr) {
    if (!this_->serialList_.empty()) {
      for (int i = 0; i < this_->serialList_.size(); i++) {
        if (this_->serialList_[i]->name_ == "fingerEventInterFacePrint" &&
            this_->serialList_[i]->isOpen_) {
          this_->serialList_[i]->sendData(data, data_len);
        }
      }
    }
  }
}

dataFingerImpl::dataFingerImpl() { this_ = this; }

dataFingerImpl::~dataFingerImpl() { serialDataCheckFlag_ = false; }

void dataFingerImpl::onInit() {
  fingerPrint_.setUARTSendData(sendFingerPrintSerialData);
  registerCallback(&fingerPrint_);
  for (auto it : serialmp_) {
    serialList_.push_back(std::make_shared<SerialTermios>(
        it.second.port, it.second.baudrate, it.first));
    if (!serialList_.back()->serialOpen()) {
      printf("%s serial open failed , %s, %d \r\n", it.first.data(),
             serialList_.back()->name_, serialList_.back()->isOpen_);
    }
  }

  serialDataCheckThread_ =
      std::make_shared<std::thread>(&dataFingerImpl::serialDataCheckRun_, this);
  serialDataCheckThread_->detach();
}

void dataFingerImpl::hasEventCallback(lcy::mutex_event_with_param& e) {
  switch (e.event_type) {
    case KEY_PRESS_EVENT: {
      if (e.param == nullptr) {
        return;
      } else {
        int press_num = *(static_cast<int*>(e.param));
        printf("finger receive press num : %d\r\n", press_num);
        if (press_num == ADD_NUM) {
          fingerPrint_.Add_FR();
        }
      }
    } break;

    default:
      break;
  }
}

void dataFingerImpl::serialDataCheckRun_() {
  while (serialDataCheckFlag_) {
    if (!serialList_.empty()) {
      for (int i = 0; i < serialList_.size(); i++) {
        if (serialList_[i]->isOpen_) {
          int readLen = serialList_[i]->receiveData(readBuf_, READ_BUF_LEN);
          // int readLen = 0;
          if (readLen > 0) {
            serial_with_params_event e;
            e.event_type = SERIAL_RECEIVE_EVENT;
            e.params = readBuf_;
            serialEvent(e);
          }
        }
      }
    }
    usleep(20 * 1000);
  }
}

void dataFingerImpl::serialEvent(serial_with_params_event& e) {
  for (auto& cb : adsCallbackInterfaces) {
    cb->hasEvent(e);
  }
