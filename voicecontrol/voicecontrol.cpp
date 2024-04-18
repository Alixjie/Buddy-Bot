#include "voicecontrol.h"
#include <QProcess>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../movecontrol/movecontrol.h"

// pulseaudio cannot run in root mode, so we cannot activate server by qt
/*
int VoiceControl::startserver(){

    QString program = sever_path ;
    server->start(program);
    return 0;
}

int VoiceControl::stopserver(){
    server->close();
    return 0;
}
*/

int VoiceControl::voicecontrol_start() {

  if (vcstopFlag.load()) {
    vcstopFlag.store(false);
    voiceThread = std::thread(&VoiceControl::voicecontrol_run, this);
  }
  return 0;
}

void VoiceControl::voicecontrol_run() {

  /*
  int status = startserver();
  if(status != 0){
      qDebug() << "Failed to start server with status:" << status;
      return -1;
  }
  */
  bool iscomeorfollow = false;

  int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    return;
  }
  Operation *recv_operation = (Operation *)mmap(
      0, sizeof(Operation), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

  sem_t *sem = sem_open(SEM_NAME, 0);
  Operation op;
  while (!vcstopFlag.load()) {
    sem_wait(sem);
    op = *recv_operation;
     printf("operation:%d\n", (int)op);
    if (op == Operation::STOP) {
      // come_stop_flag = true;
      // follow_stop_flag = true;
      ULM3Samples::getInstance().stopFollow();
      MoveControl::getInstance().Stop();
      iscomeorfollow = false;
    } else {
      if (iscomeorfollow) { // normal operation cannot be executed when come or
                            // follow is running
        continue;
      } else {
        MoveControl::getInstance().SetFromOperation(op);
        if (op == Operation::FOLLOW) {
          iscomeorfollow = true; // nedd to fix
        }
      }
    }
  }

  MoveControl::getInstance().Stop();

  munmap(recv_operation, sizeof(Operation));
  close(shm_fd);
  sem_close(sem);
}

int VoiceControl::voicecontrol_stop() {
  if (vcstopFlag.load()) {
    return 0;
  }
  vcstopFlag.store(true);
  voiceThread.join();
  // stopserver();
  return 0;
}
