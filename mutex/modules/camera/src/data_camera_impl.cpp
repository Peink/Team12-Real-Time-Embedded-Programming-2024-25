#include "data_camera_impl.hpp"

#include <assert.h>
#include <unistd.h>

#include "shm.hpp"
#include "zmq_publisher.hpp"

#define FILE_BUF 65536

#ifdef USE_ARM
#define DEV_NAME "DEV_ARM"
#else
#define DEV_NAME "DEV_X86"
#endif

dataCameraImpl::dataCameraImpl() {}

dataCameraImpl::~dataCameraImpl() { cameraDataFlag_ = false; }

void dataCameraImpl::onInit() {
  int width = 640;
  int height = 480;
  camera_.onInit("/dev/video0", height, width);
  data_deBugView_interface_.onInit("5557");
  order_interface_.onInit("5558");

  cameraDataThread_ =
      std::make_shared<std::thread>(&dataCameraImpl::cameraDataRun_, this);
  cameraDataThread_->detach();
}

void dataCameraImpl::hasEventCallback(lcy::mutex_event_with_param& e) {
  switch (e.event_type) {
    case ECHO_DISTANCE_DANGER_EVENT: {
      std::string capture_order = "start_capture";
      order_interface_.write(capture_order.data(), capture_order.size());
    } break;

    default:
      break;
  }
}

#ifdef USE_ARM
void dataCameraImpl::toChar(cv::Mat* out_mat_) {
  // cv::Mat* rgb_frame = &vis_.out_mat;
  cv::Mat* rgb_frame = out_mat_;
  // Convert the RGB Mat object to a char array
  // printf("rows: %d, cols: %d, channels: %d\r\n", rgb_frame->rows,
  //        rgb_frame->cols, rgb_frame->channels());
  uint32_t rows = rgb_frame->rows;
  uint32_t cols = rgb_frame->cols;
  uint32_t channels = rgb_frame->channels();
  // uint32_t dataSize = rgb_frame.channels() * rgb_frame.rows *
  // rgb_frame.cols; Iterate through the Mat object and copy the data into a char array

  for (int y = 0; y < rows; ++y) {
    for (int x = 0; x < cols; ++x) {
      for (int c = 0; c < channels; ++c) {
        // Calculate the index of the current pixel and channel
        size_t index = y * cols * channels + x * channels + c;
        // Get the data in the Mat object and convert it to char type
        ImgRgbData_.push_back(
            static_cast<char>(rgb_frame->at<cv::Vec3b>(y, x)[c]));
      }
    }
  }
}
#endif

#define SENDDATALEN 480 * 640 * 3 * 2 + sizeof(uint16_t)
static char sendData[SENDDATALEN] = {0};
static uint16_t numss = 0;

void dataCameraImpl::cameraDataRun_() {
  camera_.startCapturing();
  while (cameraDataFlag_) {
    if (camera_.readFrame()) {
#ifdef USE_ARM
      ImgRgbData_.clear();
      cv::Mat init_mat = camera_.getRgbMat();
      toChar(&init_mat);
      memset(sendData, 0, SENDDATALEN);
      memcpy(sendData, &numss, sizeof(uint16_t));
      memcpy(sendData + sizeof(uint16_t), ImgRgbData_.data(),
             ImgRgbData_.size());
      data_deBugView_interface_.write(sendData, SENDDATALEN);
      numss++;
#endif
    }
    usleep(5 * 1000);
  }
  camera_.stopCapturing();
}