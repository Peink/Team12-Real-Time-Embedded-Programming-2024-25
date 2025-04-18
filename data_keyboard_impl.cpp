#include "data_keyboard_impl.hpp"

static std::vector<std::vector<int>> press_num =
    std::vector<std::vector<int>>(28, std::vector<int>(28, -1));

static keyBoardEventInterFace keyboard_;

dataKeyBoardImpl::dataKeyBoardImpl() {}

dataKeyBoardImpl::~dataKeyBoardImpl() {
  checkKeyBoardRunFlag_ = false;
  releaseGpioSrc();
}

void dataKeyBoardImpl::releaseGpioSrc() {
  for (int i = 0; i < gpio_c.size(); i++) {
    if (gpio_c[i].line != nullptr) gpiod_line_release(gpio_c[i].line);
  }

  for (int i = 0; i < gpio_r.size(); i++) {
    if (gpio_r[i].line != nullptr) gpiod_line_release(gpio_r[i].line);
  }
  usleep(1000);
}

void dataKeyBoardImpl::setOutputHight(gpiod_chip *chip, gpio_t *gpio_) {
  gpio_->line = gpiod_chip_get_line(chip, gpio_->line_num);
  if (!gpio_->line) {
    printf("Get line failed\n");
  }

  // 请求GPIO线为输出模式
  if (gpiod_line_request_output(gpio_->line, CONSUMER, 0) < 0) {
    perror("Failed to request GPIO line as output");
  }

  // 将GPIO线设置为高电平
  if (gpiod_line_set_value(gpio_->line, 1) < 0) {
    perror("Failed to set GPIO line to high");
  }

  // printf("GPIO %d set to high\n", gpio_->line_num);
}

void dataKeyBoardImpl::setEventListen(gpiod_chip *chip, gpio_t *gpio_) {
  gpio_->line = gpiod_chip_get_line(chip, gpio_->line_num);
  if (!gpio_->line) {
    printf("Get line failed\n");
  }
  int ret;
  ret = gpiod_line_request_rising_edge_events(gpio_->line, CONSUMER);
  if (ret < 0) {
    printf("Request line as rising edge failed\n");
  }
}

void dataKeyBoardImpl::startListen(gpiod_chip *chip, gpio_t *gpio_,
                                   gpio_check_t *gpio_check_) {
  /* Notify event up to 20 times */
  int ret, j = 0;
  int count = 0;
  int last_num = 0;
  int val = 0;
  gpio_->line = gpiod_chip_get_line(chip, gpio_->line_num);
  if (!gpio_->line) {
    printf("Get line failed\n");
  }
  const timespec ts = {0, 1000000};
  val = gpiod_line_event_wait(gpio_->line, &ts);
  gpio_->val = val;
  keyboard_event_with_params event;
  event_param_keyboard event_param_keyboard_;
  event_param_keyboard_.gpio_ = gpio_;
  event_param_keyboard_.gpio_check_ = gpio_check_;
  event.params = &event_param_keyboard_;
  if (val < 0) {
    perror("Read line input failed\n");
    return;
  } else if (val > 0) {
    gpiod_line_event_read(gpio_->line, &event.event);
  } else if (val == 0) {  // time out
    event.event.event_type = GPIOD_LINE_EVENT_FALLING_EDGE;
  }
  keyBoardEvent(event);
  usleep(1000);
}

void dataKeyBoardImpl::onInit() {
  press_num[17][24] = 1;
  press_num[27][24] = 2;
  press_num[22][24] = 3;
  press_num[18][24] = 4;

  press_num[17][23] = 5;
  press_num[27][23] = 6;
  press_num[22][23] = 7;
  press_num[18][23] = 8;

  press_num[17][11] = 9;
  press_num[27][11] = 10;
  press_num[22][11] = 11;
  press_num[18][11] = 12;

  press_num[17][26] = 13;
  press_num[27][26] = 14;
  press_num[22][26] = 15;
  press_num[18][26] = 16;

  chip = gpiod_chip_open_by_name(chipname);
  if (!chip) {
    printf("Open chip failed\n");
    ret = -1;
  }

  releaseGpioSrc();

  registerCallback(&keyboard_);

  keyBoardCheckThread_ =
      std::make_shared<std::thread>(&dataKeyBoardImpl::start, this);
  keyBoardCheckThread_->detach();
}

void dataKeyBoardImpl::hasEventCallback(lcy::mutex_event_with_param &e) {}

void dataKeyBoardImpl::start() {
  while (checkKeyBoardRunFlag_) {
    for (int i = 0; i < gpio_r.size(); i++) {
      setEventListen(chip, &gpio_r[i]);
    }
    usleep(1000);
    for (int i = 0; i < gpio_c.size(); i++) {
      setOutputHight(chip, &gpio_c[i]);
    }
    usleep(1000);
    for (int i = 0; i < gpio_r.size(); i++) {
      startListen(chip, &gpio_r[i], &row);
    }

    releaseGpioSrc();

    for (int i = 0; i < gpio_c.size(); i++) {
      setEventListen(chip, &gpio_c[i]);
    }
    usleep(1000);
    for (int i = 0; i < gpio_r.size(); i++) {
      setOutputHight(chip, &gpio_r[i]);
    }
    usleep(1000);
    for (int i = 0; i < gpio_c.size(); i++) {
      startListen(chip, &gpio_c[i], &column);
    }
    releaseGpioSrc();

    if (column.final_record < 28 && column.final_record > 0 &&
        row.final_record < 28 && row.final_record > 0) {
      if (press_num[column.final_record][row.final_record] != -1) {
        printf("press : %d\r\n",
               press_num[column.final_record][row.final_record]);
        int press_event_num = press_num[column.final_record][row.final_record];
        lcy::mutex_event_with_param e;
        e.event_type = KEY_PRESS_EVENT;
        e.param = &press_event_num;
        eventLoop(e);
        column.final_record = -1;
        row.final_record = -1;
      }
    }
  }
}

void dataKeyBoardImpl::keyBoardEvent(keyboard_event_with_params &e) {
  for (auto &cb : adsCallbackInterfaces) {
    cb->hasEvent(e);
  }
}

keyBoardEventInterFace::keyBoardEventInterFace() {}

void keyBoardEventInterFace::hasEvent(
    dataKeyBoardImpl::keyboard_event_with_params &e) {
  event_param_keyboard *event_keyboard_params =
      static_cast<event_param_keyboard *>(e.params);
  gpio_t *gpio_ = static_cast<gpio_t *>(event_keyboard_params->gpio_);
  gpio_check_t *gpio_check_ =
      static_cast<gpio_check_t *>(event_keyboard_params->gpio_check_);
  switch (e.event.event_type) {
    case GPIOD_LINE_EVENT_RISING_EDGE: {
      gpio_check_->num = gpio_->line_num;
      if (gpio_check_->last_num == gpio_check_->num) {
        gpio_check_->count++;
        if (gpio_check_->count >= 3) {
          gpio_check_->record = gpio_check_->num;
          gpio_check_->count = 0;
          gpio_check_->last_num = -1;
          // printf("rise num :%d\r\n", gpio_check_->record);
        }
      } else {
        gpio_check_->count = 0;
      }
      gpio_check_->last_num = gpio_check_->num;
    } break;

    case GPIOD_LINE_EVENT_FALLING_EDGE: {
      if (gpio_check_->record != -1)
        // printf("down num :%d, record num :%d \r\n", gpio_->line_num,
        //        gpio_check_->record);
        if (gpio_check_->record != -1 &&
            gpio_check_->record == gpio_->line_num) {
          gpio_check_->down_count++;
          if (gpio_check_->down_count >= 3) {
            gpio_check_->final_record = gpio_check_->record;
            gpio_check_->down_count = 0;
            gpio_check_->record = -1;
            // printf("final_record: %d\r\n", gpio_check_->final_record);
          }
        }
    } break;

    default: {
    } break;
  }
}