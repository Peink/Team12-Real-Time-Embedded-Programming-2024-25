#include "data_echo_impl.hpp"

static struct gpiod_chip *chip;
static echoEventInterFace echoEventInterFace_;
static uint8_t distance = 0;
static dataEchoImpl *dataEchoImpl_ = nullptr;
static bool echo_trig_flag = false;
static bool echo_end_flag = false;

dataEchoImpl::dataEchoImpl() { dataEchoImpl_ = this; }
// Stop the echo check thread loop
dataEchoImpl::~dataEchoImpl() { echoDataCheckFlag_ = false; }


void dataEchoImpl::onInit() {
  int ret;

  char *chipname = CHIP_NAME;
  chip = gpiod_chip_open_by_name(chipname);
  if (!chip) {
    printf("Open chip failed\n");
    return;
  }

  // Set "bing" as HIGH, "trig" as LOW, "echo" as input
  setOutputHight(chip, &bing_gpio_);
  setOutputLow(chip, &trig_gpio_);
  setEventListen(chip, &echo_gpio_);

  //Release lines after setup
  gpiod_line_release(bing_gpio_.line);
  gpiod_line_release(trig_gpio_.line);
  
 // Register a callback handler for echo events
  registerCallback(&echoEventInterFace_);
  
 //Start a dedicated thread to repeatedly trigger and listen for echoes
  echoDataCheckThread_ =
      std::make_shared<std::thread>(&dataEchoImpl::echoDataCheckRun_, this);

  echoDataCheckThread_->detach();
}

void dataEchoImpl::hasEventCallback(lcy::mutex_event_with_param &e) {
  switch (e.event_type) {
    case KEY_PRESS_EVENT: {
      buzzerStart();
      usleep(500 * 1000); //500ms
      buzzerEnd();
    } break;

    default:
      break;
  }
}

void dataEchoImpl::echoDataCheckRun_() {
  while (echoDataCheckFlag_) {
    if (echo_trig_flag == false) {
      Trig();
      echo_trig_flag = true;
    }
    startEventListen(chip, &echo_gpio_);
  }
}

//echo event broadcaster
void dataEchoImpl::echoEvent(echo_event &e) {
  for (auto &cb : adsCallbackInterfaces) {
    cb->hasEvent(e);
  }
}

// GPIO control function
void dataEchoImpl::setOutputHight(gpiod_chip *chip, gpio_t *gpio_) {
  gpio_->line = gpiod_chip_get_line(chip, gpio_->line_num);
  if (!gpio_->line) {
    printf("Get line failed\n");
  }

  // Request the GPIO line as output
  if (gpiod_line_request_output(gpio_->line, CONSUMER, 0) < 0) {
    perror("Failed to request GPIO line as output");
  }

  // set GPIO line to HIGH
  if (gpiod_line_set_value(gpio_->line, 1) < 0) {
    perror("Failed to set GPIO line to high");
  }

  // printf("GPIO %d set to high\n", gpio_->line_num);
}

void dataEchoImpl::setOutputLow(gpiod_chip *chip, gpio_t *gpio_) {
  gpio_->line = gpiod_chip_get_line(chip, gpio_->line_num);
  if (!gpio_->line) {
    printf("Get line failed\n");
  }

  // Request the GPIO line as output
  if (gpiod_line_request_output(gpio_->line, CONSUMER, 1) < 0) {
    perror("Failed to request GPIO line as output");
  }

  // set GPIO line to LOW
  if (gpiod_line_set_value(gpio_->line, 0) < 0) {
    perror("Failed to set GPIO line to low");
  }
}

void dataEchoImpl::setEventListen(gpiod_chip *chip, gpio_t *gpio_) {
  gpio_->line = gpiod_chip_get_line(chip, gpio_->line_num);
  if (!gpio_->line) {
    printf("Get line failed\n");
  }
  int ret;
  ret = gpiod_line_request_input(gpio_->line, CONSUMER);
  if (ret < 0) {
    printf("Request line as input failed\n");
  }
}

void dataEchoImpl::startEventListen(gpiod_chip *chip, gpio_t *gpio_) {
  int ret, j = 0;
  int count = 0;
  int last_num = 0;
  int val = 0;
   //Reads the current value and interprets 
  gpio_->line = gpiod_chip_get_line(chip, gpio_->line_num);
  if (!gpio_->line) {
    printf("Get line failed\n");
  }
  
  echo_event e;
  val = gpiod_line_get_value(gpio_->line);
  if (val < 0) {
    perror("gpiod_line_get_value");
    return;
  } else if (val == 0) {
    e.event.event_type = GPIOD_LINE_EVENT_FALLING_EDGE;
  } else if (val > 0) {
    e.event.event_type = GPIOD_LINE_EVENT_RISING_EDGE;
  }
  echoEvent(e);
}


//Ultrasonic Sensor Trigger and Buzzer control
void dataEchoImpl::Trig() {
  // Create a short HIGH pulse on the trigger line
  setOutputHight(chip, &trig_gpio_);
  gpiod_line_release(trig_gpio_.line);
  usleep(200);
  setOutputLow(chip, &trig_gpio_);
  gpiod_line_release(trig_gpio_.line);
}

void dataEchoImpl::buzzerStart() {
  // Some buzzers/relays are active-low, so set LOW
  setOutputLow(chip, &bing_gpio_);
  gpiod_line_release(bing_gpio_.line);
  printf("up\r\n");
}

void dataEchoImpl::buzzerEnd() {
  // Deactivate the buzzer by setting line HIGH
  setOutputHight(chip, &bing_gpio_);
  gpiod_line_release(bing_gpio_.line);
  printf("down \r\n");
}

void echoEventInterFace::hasEvent(dataEchoImpl::echo_event &e) {
  switch (e.event.event_type) {
    case GPIOD_LINE_EVENT_FALLING_EDGE: {
      if (echo_end_flag) {
        echo_end_flag = false; //End of the pulse detected
        printf("triger, distance : %d\r\n", distance);
       
        echo_trig_flag = false; // Reset the trigger flag so the sensor can fire again
        
        //Example logic to activate buzzer if too close,
        // if (distance <= 10) {
        //   if (dataEchoImpl_) dataEchoImpl_->buzzerStart();
        // } else {
        //   if (dataEchoImpl_) dataEchoImpl_->buzzerEnd();
        // }
        
        distance = 0;
        sleep(1);
      }
    } break;
    case GPIOD_LINE_EVENT_RISING_EDGE: {
      echo_end_flag = true; //Begin of the pulse
      distance++; // Increment the distance counter
      usleep(2);
    } break;

    default:
      break;
  }
}
