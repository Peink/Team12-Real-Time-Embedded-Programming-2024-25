#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <string>
#include <vector>

class Camera {
 public:
  Camera();
  Camera(const char* deviceName, int height, int width);
  ~Camera();

  // int onInit();
  int onInit(const char* deviceName, int height, int width);
  void startCapturing();
  void stopCapturing();
  bool readFrame();

  int MJPEGConvertRGB();

  uint32_t getImgRgbSize();
  const char* getImgRgb();

  cv::Mat getRgbMat();
  cv::Mat getBgrMat();

 private:
  std::string deviceName_;
  int fd_;
  unsigned char* mptr_[4];
  unsigned int size_[4];  // 存储大小，方便释放
  struct v4l2_buffer readbuffer_;
  int height_;
  int width_;
  int bufferCount_;

  char* ImgRgbData_;
  uint32_t ImgRgbDataSize_;
  cv::Mat rgb_frame_;
  cv::Mat mjpeg_frame;
};

#endif