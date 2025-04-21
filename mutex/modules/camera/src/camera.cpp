#include "camera.hpp"

Camera::Camera(const char* deviceName, int height, int width)
    : fd_(-1), bufferCount_(0), height_(height), width_(width) {
  deviceName_ = std::string(deviceName);
  ImgRgbDataSize_ = height * width * 3;
  ImgRgbData_ = new char[ImgRgbDataSize_];
}

uint32_t Camera::getImgRgbSize() { return ImgRgbDataSize_; }

const char* Camera::getImgRgb() { return ImgRgbData_; }

int Camera::onInit(const char* deviceName, int height, int width) {
  height_ = height;
  width_ = width;

  deviceName_ = std::string(deviceName);
  ImgRgbDataSize_ = height * width * 3;
  ImgRgbData_ = new char[ImgRgbDataSize_];

  // Open the camera device
  fd_ = open(deviceName_.data(), O_RDWR);
  if (fd_ == -1) {
    throw std::runtime_error("Failed to open camera device");
  }

  struct v4l2_capability cap = {0};
  if (ioctl(fd_, VIDIOC_QUERYCAP, &cap) == -1) {
    close(fd_);
    throw std::runtime_error("Failed to query camera capabilities");
  }

  /*Set the video format*/
  struct v4l2_format fmt = {0};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = width_;
  fmt.fmt.pix.height = height_;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;
  if (ioctl(fd_, VIDIOC_S_FMT, &fmt) == -1) {
    close(fd_);
    throw std::runtime_error("Failed to set video format");
  }

  memset(&fmt, 0, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(fd_, VIDIOC_G_FMT, &fmt) < 0) {
    perror("Get format failed:");
    return -1;
  }

  if (fmt.fmt.pix.width == width_ && fmt.fmt.pix.height == height_ &&
      fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_MJPEG) {
    // LOG_INFO("Set successful!\n");
  } else {
    // LOG_ERROR("Set failed!\n");
    return -1;
  }

  /*Apply for kernel buffer queue*/
  struct v4l2_requestbuffers reqbuffer;
  reqbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  reqbuffer.count = 4;
  reqbuffer.memory = V4L2_MEMORY_MMAP;  // Mapping method
  if (ioctl(fd_, VIDIOC_REQBUFS, &reqbuffer) < 0) {
    perror("Request Queue space failed:");
    return -1;
  }

  /*Mapping to user space*/
  struct v4l2_buffer mapbuffer;
  int i;
  mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  for (i = 0; i < 4; i++) {
    mapbuffer.index = i;
    if (ioctl(fd_, VIDIOC_QUERYBUF, &mapbuffer) < 0) {
      perror("Kernel space queue failed:");
      return -1;
    }
    mptr_[i] =
        (unsigned char*)mmap(NULL, mapbuffer.length, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd_, mapbuffer.m.offset);
    size_[i] = mapbuffer.length;
    // Finished using
    if (ioctl(fd_, VIDIOC_QBUF, &mapbuffer) < 0) {
      perror("Return failed:");
      return -1;
    }
  }

  // LOG_INFO("Camera init success");
  return 0;
}

void Camera::startCapturing() {
  // Start video streaming
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(fd_, VIDIOC_STREAMON, &type) == -1) {
    throw std::runtime_error("Failed to start capturing");
  }
}

void Camera::stopCapturing() {
  // Stop video streaming
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(fd_, VIDIOC_STREAMOFF, &type) == -1) {
    throw std::runtime_error("Failed to stop capturing");
  }
}

bool Camera::readFrame() {
  // Read and write camera frame data from memory map
  readbuffer_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(fd_, VIDIOC_DQBUF, &readbuffer_) < 0) {
    perror("Capture failed:");
    return false;
  }
  // Display on LCD
  // LOG_INFO("read frame success, index : {}, lenght: {}", readbuffer_.index,
  //          size_[readbuffer_.index]);
  if (!MJPEGConvertRGB()) {
    // LOG_INFO("MJEPG convert to RBG success");
  } else {
    // LOG_ERROR("MJEPG convert to RBG failed");
    return false;
  }
  // Notify the kernel that it has finished using
  if (ioctl(fd_, VIDIOC_QBUF, &readbuffer_) < 0) {
    perror("return failed:");
    return false;
  }
  return true;
}

int Camera::MJPEGConvertRGB() {
  // Convert MJPEG data to OpenCV Mat object
  mjpeg_frame = cv::imdecode(
      cv::Mat(1, size_[readbuffer_.index], CV_8UC1, mptr_[readbuffer_.index]),
      cv::IMREAD_COLOR);
  if (mjpeg_frame.empty()) {
    std::cerr << "Unable to decode MJPEG data into Mat object" << std::endl;
    return -1;
  }
  // 

  cv::cvtColor(mjpeg_frame, rgb_frame_, cv::COLOR_BGR2RGB);

  return 0;
}

cv::Mat Camera::getRgbMat() { return rgb_frame_; }

cv::Mat Camera::getBgrMat() { return mjpeg_frame; }

Camera::Camera() = default;

Camera::~Camera() {
  if (fd_ != -1) {
    stopCapturing();
    // Release the mapping space
    for (int i = 0; i < 4; i++) {
      munmap(mptr_[i], size_[i]);
    }
    close(fd_);
  }
}
