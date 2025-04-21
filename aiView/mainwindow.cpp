#include "mainwindow.h"

#include <unistd.h>

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QTimeZone>
#include <chrono>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  new_mat = cv::Mat(ROW, COL, cv::COLOR_BGR2RGB);
  init_mat = cv::Mat(ROW, COL, cv::COLOR_BGR2RGB);
  ui->setupUi(this);
  data_interface_ = std::make_unique<data_interface>();
  data_interface_->init();

  view_ = std::make_unique<std::thread>(&MainWindow::viewRun, this);
  view_->detach();
  this->setFixedSize(640, 480);
}

MainWindow::~MainWindow() { delete ui; }

static int8_t picture_count = 0;

void MainWindow::capture_picture() {
  if (capture_flag) {
    picture_count = 10;
    capture_flag = false;
  }
  if (picture_count > 0) {
    picture_count--;
    on_pushButton_clicked();
  }
}

void MainWindow::showmat() {
  std::unique_lock<std::mutex> locker(data_mutex);
  uint16_t num = 0;
  memcpy(&num, recv_data, sizeof(uint16_t));
  // Copy the data from ImgRgbData_ to new_mat
  for (int y = 0; y < ROW; ++y) {
    for (int x = 0; x < COL; ++x) {
      // Calculate the starting index of the current pixel
      size_t index = y * COL * CHANNEL;
      // Copy pixel values ​​to new_mat
      new_mat.at<cv::Vec3b>(y, x)[0] =
          (recv_data + sizeof(uint16_t))[index + x * CHANNEL + 0];  // B
      new_mat.at<cv::Vec3b>(y, x)[1] =
          (recv_data + sizeof(uint16_t))[index + x * CHANNEL + 1];  // G
      new_mat.at<cv::Vec3b>(y, x)[2] =
          (recv_data + sizeof(uint16_t))[index + x * CHANNEL + 2];  // R
    }
  }

  for (int y = 0; y < ROW; ++y) {
    for (int x = 0; x < COL; ++x) {
      // Calculate the starting index of the current pixel
      size_t index = y * COL * CHANNEL;
      // Copy pixel values ​​to init_mat
      init_mat.at<cv::Vec3b>(y, x)[0] =
          (recv_data + sizeof(uint16_t) +
           COL * ROW * CHANNEL)[index + x * CHANNEL + 0];  // B
      init_mat.at<cv::Vec3b>(y, x)[1] =
          (recv_data + sizeof(uint16_t) +
           COL * ROW * CHANNEL)[index + x * CHANNEL + 1];  // G
      init_mat.at<cv::Vec3b>(y, x)[2] =
          (recv_data + sizeof(uint16_t) +
           COL * ROW * CHANNEL)[index + x * CHANNEL + 2];  // R
    }
  }

  locker.unlock();

  // Get the current UTC time
  QDateTime utcNow = QDateTime::currentDateTimeUtc();

  // Convert to millisecond timestamp (milliseconds since epoch)
  qint64 utcTimestampMillis = utcNow.toMSecsSinceEpoch();

  // Print UTC timestamp
  qDebug() << "UTC Timestamp (milliseconds since epoch):" << utcTimestampMillis;

  img = QImage(new_mat.data, new_mat.cols, new_mat.rows,
               static_cast<int>(new_mat.step), QImage::Format_RGB888);
  std::unique_lock<std::mutex> img_locker(img_mutex_);
  // If you also need formatted date and time strings, you can use the following method
  utcDateTimeString = utcNow.toString("yyyy-MM-dd HH:mm:ss.zzz");
  qDebug() << "UTC DateTime String:" << utcDateTimeString;
  init_img = QImage(init_mat.data, init_mat.cols, init_mat.rows,
                    static_cast<int>(init_mat.step), QImage::Format_RGB888);
  img_locker.unlock();
  this->ui->PictureView->setPixmap(QPixmap::fromImage(img));
  this->ui->PictureView->resize(this->ui->PictureView->pixmap()->size());
}

void MainWindow::viewRun() {
  sleep(2);
  while (1) {
    showmat();
    capture_picture();
    usleep(10000);
  }
}

static QString picture_path;
void MainWindow::on_pushButton_clicked() {
  std::unique_lock<std::mutex> locker(img_mutex_);
  picture_path = "picture";
  picture_path += utcDateTimeString;
  picture_path += ".png";
  qDebug() << picture_path;
  img.save(picture_path, "png");
  locker.unlock();
  qDebug() << "save "
              "success========================================================="
              "=====================";
}
