#ifndef _DATA_FINGER_IMPL_H_
#define _DATA_FINGER_IMPL_H_

#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "mutex_node.hpp"
#include "serialInterface.hpp"

#define READ_BUF_LEN 2048

class dataFingerImpl : public lcy::mutexNode {
 public:
  dataFingerImpl();
  ~dataFingerImpl();

  char readBuf_[READ_BUF_LEN] = {0};

  void onInit() override;

  typedef struct {
    std::string port;
    int baudrate;
  } serialParam;

  typedef enum {
    SERIAL_RECEIVE_EVENT = 0,
  } SERIAL_EVENT;

  typedef struct {
    SERIAL_EVENT event_type;
    void* params;
  } serial_with_params_event;

  class SerialEventCallbackInterface {
    /**
     * Called when a new sample is available.
     * This needs to be implemented in a derived
     * class by the client. Defined as abstract.
     * \param e If serial receive data
     **/
   public:
    virtual void hasEvent(serial_with_params_event& e) = 0;
  };

  void registerCallback(SerialEventCallbackInterface* ci) {
    adsCallbackInterfaces.push_back(ci);
  }

  std::vector<std::shared_ptr<serialInterface>> serialList_;

 private:
  std::unordered_map<std::string, serialParam> serialmp_ = {
      {"fingerEventInterFacePrint",
       {.port = "/dev/ttyAMA0", .baudrate = 115200}},
  };
  std::shared_ptr<std::thread> serialDataCheckThread_;
  void serialDataCheckRun_();
  bool serialDataCheckFlag_ = true;

  std::vector<SerialEventCallbackInterface*> adsCallbackInterfaces;

  void serialEvent(serial_with_params_event& e);
};

#endif