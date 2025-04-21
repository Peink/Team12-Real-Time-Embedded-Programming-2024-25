#ifndef _SHM_H_
#define _SHM_H_

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <memory>
#include <string>

#define SHM_FILE "/shared_memory"
#define SEM_WRITE_FILE "/shared_write_sem"
#define SEM_READ_FILE "/shared_read_sem"

class Shm {
 public:
  Shm() = default;
  ~Shm() = default;

  Shm(size_t size);

  void onInit(int size);

  void write(const char* data, int len);
  void read(char* data, int len);

 private:
  size_t Shm_size_;  // 共享内存数据块大小

  int fd_;  //>共享内存的文件描述符
  char* shared_mem_ = nullptr;
  sem_t* write_sem_ = nullptr;
  sem_t* read_sem_ = nullptr;
};

#endif