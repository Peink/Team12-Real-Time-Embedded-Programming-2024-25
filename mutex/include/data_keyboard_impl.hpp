#ifndef _DATA_KEYBOARD_IMPL_H_
#define _DATA_KEYBOARD_IMPL_H_

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

#include <memory>
#include <thread>
#include <vector>

#include "mutex_node.hpp"

#ifndef CONSUMER
#define CONSUMER "Consumer"
#endif

#define C1_NUM 18
#define C2_NUM 22
#define C3_NUM 27
#define C4_NUM 17

#define R1_NUM 26
#define R2_NUM 11
#define R3_NUM 23
#define R4_NUM 24

#define CHIP_NAME "gpiochip0"

typedef struct {
  unsigned int line_num;
  struct gpiod_line *line;
  int val;
} gpio_t;

typedef struct {
  int last_num;
  int num;
  int count;
  int down_count;
  int record;
  int final_record;
} gpio_check_t;

typedef struct {
  gpio_t *gpio_;
  gpio_check_t *gpio_check_;
} event_param_keyboard;

typedef struct {
  gpiod_line_event event;
  void *params;
} event_with_params;

class dataKeyBoardImpl : public lcy::mutexNode {
 public:
  dataKeyBoardImpl();
  ~dataKeyBoardImpl();

  dataKeyBoardImpl(lcy::mutexNode *node) : node_(node) {}

  void onInit() override;
  void start();

 private:
  gpio_check_t row;
  gpio_check_t column;

  gpio_t c1 = {C1_NUM, nullptr};
  gpio_t c2 = {C2_NUM, nullptr};
  gpio_t c3 = {C3_NUM, nullptr};
  gpio_t c4 = {C4_NUM, nullptr};

  gpio_t r1 = {R1_NUM, nullptr};
  gpio_t r2 = {R2_NUM, nullptr};
  gpio_t r3 = {R3_NUM, nullptr};
  gpio_t r4 = {R4_NUM, nullptr};

  struct gpiod_chip *chip;
  struct gpiod_line *line;
  int ret;

  std::vector<gpio_t> gpio_c = {c1, c2, c3, c4};
  std::vector<gpio_t> gpio_r = {r1, r2, r3, r4};

  char *chipname = CHIP_NAME;

  void setOutputHight(gpiod_chip *chip, gpio_t *gpio_);
  void setEventListen(gpiod_chip *chip, gpio_t *gpio_);
  void startListen(gpiod_chip *chip, gpio_t *gpio_, gpio_check_t *gpio_check_);
  void releaseGpioSrc();

  void keyBoardScanCallback(event_with_params &event);

  lcy::mutexNode *node_;
  bool checkKeyBoardRunFlag_ = true;
  std::shared_ptr<std::thread> keyBoardCheckThread_;
};

#endif