#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <thread>

#include "data.hpp"

#define ROW 480
#define COL 640
#define CHANNEL 3

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void showmat();
  void capture_picture();

 private slots:

  void on_pushButton_clicked();

 private:
  Ui::MainWindow *ui;
  std::unique_ptr<data_interface> data_interface_;
  std::unique_ptr<std::thread> view_;
  void viewRun();
  cv::Mat new_mat;
  cv::Mat init_mat;
  QImage img;
  QImage init_img;
  std::mutex img_mutex_;
  QString utcDateTimeString;
};
#endif  // MAINWINDOW_H
