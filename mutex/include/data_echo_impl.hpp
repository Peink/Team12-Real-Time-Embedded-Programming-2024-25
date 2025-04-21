#ifndef _DATA_ECHO_IMPL_H_
#define _DATA_ECHO_IMPL_H_

#include <gpiod.h>
#include <unistd.h>

#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "mutex_node.hpp"

#define ECHO_PIN 6
#define TRIG_PIN 16
#define BING_PIN 2
#define CHIP_NAME "gpiochip0"

#ifndef CONSUMER
#define CONSUMER "Consumer"
#endif

class dataEchoImpl : public lcy::mutexNode {
 public:
  dataEchoImpl();
  ~dataEchoImpl();

  void onInit() override;
  void hasEventCallback(lcy::mutex_event_with_param &e) override;

  typedef struct {
    gpiod_line_event event;
  } echo_event;

  class EchoEventCallbackInterface {
    /**
     * Called when a new sample is available.
     * This needs to be implemented in a derived
     * class by the client. Defined as abstract.
     * \param e If serial receive data
     **/
   public:
    virtual void hasEvent(echo_event &e) = 0;
  };

  void registerCallback(EchoEventCallbackInterface *ci) {
    adsCallbackInterfaces.push_back(ci);
  }

  void buzzerStart();
  void buzzerEnd();

 private:
  std::shared_ptr<std::thread> echoDataCheckThread_;
  void echoDataCheckRun_();
  bool echoDataCheckFlag_ = true;

  std::vector<EchoEventCallbackInterface *> adsCallbackInterfaces;

  void echoEvent(echo_event &e);

  gpio_t echo_gpio_ = {ECHO_PIN, nullptr};
  gpio_t trig_gpio_ = {TRIG_PIN, nullptr};
  gpio_t bing_gpio_ = {BING_PIN, nullptr};

  void setOutputLow(gpiod_chip *chip, gpio_t *gpio_);
  void setOutputHight(gpiod_chip *chip, gpio_t *gpio_);
  void setEventListen(gpiod_chip *chip, gpio_t *gpio_);
  void startEventListen(gpiod_chip *chip, gpio_t *gpio_);

  void Trig();
};

class echoEventInterFace : public dataEchoImpl::EchoEventCallbackInterface {
 public:
  echoEventInterFace() = default;
  ~echoEventInterFace() = default;

  void hasEvent(dataEchoImpl::echo_event &e) override;

  bool start_catch_picture_flag = true;
};

#endif