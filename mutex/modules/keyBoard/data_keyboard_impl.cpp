#include "data_keyboard_impl.hpp"

#define ADD_NUM 13
#define DELETE_NUM 14
#define VERIFICATION_NUM 15

static std::vector<std::vector<int>> press_num =
    std::vector<std::vector<int>>(28, std::vector<int>(28, -1));

static keyBoardEventInterFace keyboard_;

dataKeyBoardImpl::dataKeyBoardImpl() {
  char password_file_path[2048] = {0};
  memset(password_file_path, 0, 2048);
  snprintf(password_file_path, 2048, "%s%s", HOME_PREFIX, PASSWORD_PATH);
  std::ifstream password_file = std::ifstream(password_file_path);
  if (!password_file.is_open()) {
    printf("no password\r\n");
    password_save = std::vector<int>(6, 0);
  } else {
    std::string read_line;
    if (std::getline(password_file, read_line)) {
      printf("read password success\r\n");
      if (read_line.size() < 6) {
        printf("password error\r\n");
        password_save = std::vector<int>(6, 0);
      } else {
        for (int i = 0; i < 6; i++) {
          password_save.push_back(read_line[i] - '0');
        }
      }
    } else {
      password_save = std::vector<int>(6, 0);
    }
  }
}

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

  // Request the GPIO line to be in output mode
  if (gpiod_line_request_output(gpio_->line, CONSUMER, 0) < 0) {
    perror("Failed to request GPIO line as output");
  }

  // Set the GPIO line to high level
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
  press_num[18][24] = 12;

  press_num[17][23] = 4;
  press_num[27][23] = 5;
  press_num[22][23] = 6;
  press_num[18][23] = 13;

  press_num[17][11] = 7;
  press_num[27][11] = 8;
  press_num[22][11] = 9;
  press_num[18][11] = 14;

  press_num[17][26] = 10;
  press_num[27][26] = 0;
  press_num[22][26] = 11;
  press_num[18][26] = 15;

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

void dataKeyBoardImpl::hasEventCallback(lcy::mutex_event_with_param &e) {
  switch (e.event_type) {
    case KEY_PRESS_EVENT: {
      if (e.param == nullptr) {
        return;
      } else {
        int press_num = *(static_cast<int *>(e.param));
        printf("keyboard receive press num : %d\r\n", press_num);
        if (press_num == CONFIRM_NUM) {
          if (modify_password_flag == 2) {
            modify_password_flag = 3;
            printf("please input password again\r\n");
            playVideo("6.wav");
          } else {
            if (modify_password_flag == 3) {
              for (int i = 0; i < 6; i++) {
                if (password_new[i] != password_new_again[i]) {
                  printf("inconsistent password\r\n");
                  playVideo("8.wav");
                  password_clear();
                  return;
                }
              }
              printf("modify password success\r\n");
              playVideo("7.wav");
              char password_file_path[2048] = {0};
              memset(password_file_path, 0, 2048);
              snprintf(password_file_path, 2048, "%s%s", HOME_PREFIX,
                       PASSWORD_PATH);
              std::ofstream password_file = std::ofstream(password_file_path);
              for (int i = 0; i < 6; i++) {
                char s[1] = {0};
                s[0] = password_new[i] + '0';
                password_file.write(s, 1);
              }
              password_save = password_new;
              password_clear();
              return;
            } else {
              for (int i = 0; i < 6; i++) {
                if (password_vec[i] != password_save[i]) {
                  printf("password is error \r\n");
                  playVideo("2.wav");
                  password_clear();
                  return;
                }
              }
              printf("password is right\r\n");
              playVideo("1.wav");
              if (modify_password_flag == 1) {
                password_clear();
                modify_password_flag = 2;
                printf("please input new password\r\n");
                playVideo("5.wav");
              }else if(modify_password_flag == 10){
                printf("start add finger\r\n");
                password_clear();
                lcy::mutex_event_with_param e;
                e.event_type = ADD_FINGER_EVENT;
                eventLoop(e);
                return;
              }else if(modify_password_flag == 11) {
                printf("start delete finger\r\n");
                password_clear();
                lcy::mutex_event_with_param e;
                e.event_type = DELETE_FINGER_EVENT;
                eventLoop(e);
              }else if(modify_password_flag == 12) {
                printf("start verification finger\r\n");
                password_clear();
                lcy::mutex_event_with_param e;
                e.event_type = VERIFICATION_FINGER_EVENT;
                eventLoop(e);
              }
              else {
                password_clear();
              }
            }
          }
        } else if (press_num == CANCEL_NUM) {
          password_clear();
          printf("password verifiy is cancel\r\n");
          playVideo("3.wav");
        } else if (press_num == MODIFY_NUM) {
          printf("start modify, please input right password\r\n");
          playVideo("4.wav");
          password_clear();
          modify_password_flag = 1;
        }else if (press_num == ADD_NUM){
          printf("Before add finger, please verify password\r\n");
          modify_password_flag = 10;
          playVideo("4.wav");
        }else if (press_num == DELETE_NUM){
          printf("Before delete finger, please verify password \r\n");
          modify_password_flag = 11;
          playVideo("4.wav");
        }else {
          if (press_num == 10) press_num = 0;
          if (modify_password_flag == 2) {
            if (password_new.size() < 6) password_new.push_back(press_num);
          } else if (modify_password_flag == 3) {
            if (password_new_again.size() < 6)
              password_new_again.push_back(press_num);
          } else {
            if (password_vec.size() == 6) {
              printf("password is full\r\n");
              password_vec.clear();
            }
            password_vec.push_back(press_num);
          }
        }
      }
    } break;

    default:
      break;
  }
}

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

void dataKeyBoardImpl::password_clear() {
  password_vec.clear();
  password_new.clear();
  password_new_again.clear();
  modify_password_flag = 0;
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
