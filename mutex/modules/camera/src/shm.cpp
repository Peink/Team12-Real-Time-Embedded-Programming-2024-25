#include "shm.hpp"

#include <iostream>

Shm::Shm(size_t size) : Shm_size_(size) {}

void Shm::onInit(int size) {
  Shm_size_ = size;
  if ((fd_ = shm_open(SHM_FILE, O_CREAT | O_RDWR, 0777)) == -1) {
    perror("Shm_open");
    exit(1);
  }

  if (ftruncate(fd_, Shm_size_) == -1) {
    perror("ftruncate");
    exit(1);
  }

  if ((shared_mem_ = (char*)mmap(NULL, Shm_size_, PROT_READ | PROT_WRITE,
                                 MAP_SHARED, fd_, 0)) == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  write_sem_ = sem_open(SEM_WRITE_FILE, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
  if (write_sem_ == SEM_FAILED) {
    if (errno == EEXIST) {
      write_sem_ = sem_open(SEM_WRITE_FILE, 1);
    }
    if (write_sem_ == SEM_FAILED) {
      perror("write sem_open:");
      write_sem_ = nullptr;
      return;
    }
  }

  read_sem_ = sem_open(SEM_READ_FILE, O_CREAT | O_EXCL | O_RDWR, 0666, 0);
  if (read_sem_ == SEM_FAILED) {
    if (errno == EEXIST) {
      read_sem_ = sem_open(SEM_READ_FILE, 0);
    }
    if (read_sem_ == SEM_FAILED) {
      perror("read sem_open:");
      read_sem_ = nullptr;
      return;
    }
  }
}

void Shm::write(const char* data, int len) {
  if (shared_mem_ != nullptr) {
    if (write_sem_ == nullptr) {
      std::cout << "sem_ is nullptr" << std::endl;
    }
    if (len > Shm_size_) len = Shm_size_;
    sem_wait(write_sem_);
    memcpy(shared_mem_, data, len);
    sem_post(read_sem_);
  } else {
    std::cout << "shared_mem is nullptr" << std::endl;
  }
}

void Shm::read(char* data, int len) {}
