#ifndef _DATA_CAMERA_IMPL_H_
#define _DATA_CAMERA_IMPL_H_

#include <unistd.h>

#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "camera.hpp"
#include "mutex_node.hpp"
#include "opencv2/core/mat.hpp"
#include "shm.hpp"
#include "zmq_publisher.hpp"

class dataCameraImpl : public lcy::mutexNode {
 public:
  dataCameraImpl();
  ~dataCameraImpl();

  void onInit() override;
  void hasEventCallback(lcy::mutex_event_with_param& e) override;

 private:
  std::shared_ptr<std::thread> cameraDataThread_;
  void cameraDataRun_();
  bool cameraDataFlag_ = true;

  Camera camera_;
  ZmqPublisher data_deBugView_interface_;
  ZmqPublisher order_interface_;

#ifdef USE_ARM
  void toChar(cv::Mat* out_mat_);
  std::string ImgRgbData_;
#endif
};

#endif