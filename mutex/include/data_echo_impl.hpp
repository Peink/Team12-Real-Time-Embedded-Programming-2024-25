#ifndef _DATA_ECHO_IMPL_H_
#define _DATA_ECHO_IMPL_H_

#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "mutex_node.hpp"

class dataEchoImpl : public lcy::mutexNode {
 public:
  dataEchoImpl();
  ~dataEchoImpl();

  void onInit() override;

  typedef enum {
    SERIAL_RECEIVE_EVENT = 0,
  } ECHO_EVENT;

  typedef struct {
    ECHO_EVENT event_type;
  } echo_event;

  class EchoEventCallbackInterface {
    /**
     * Called when a new sample is available.
     * This needs to be implemented in a derived
     * class by the client. Defined as abstract.
     * \param e If serial receive data
     **/
   public:
    virtual void hasEvent(echo_event& e) = 0;
  };

  void registerCallback(EchoEventCallbackInterface* ci) {
    adsCallbackInterfaces.push_back(ci);
  }

 private:
  std::shared_ptr<std::thread> echoDataCheckThread_;
  void echoDataCheckRun_();
  bool echoDataCheckFlag_ = true;

  std::vector<EchoEventCallbackInterface*> adsCallbackInterfaces;

  void echoEvent(echo_event& e);
};

#endif