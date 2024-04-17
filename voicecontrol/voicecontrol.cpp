#include "voicecontrol.h"
#include <QProcess>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>

#include "../movecontrol/movecontrol.h"


//pulseaudio cannot run in root mode, so we cannot activate server by qt
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
int VoiceControl::voicecontrol_start(){

    bool iscomeorfollow = false;

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(Operation));
    Operation* send_operation = (Operation*)mmap(0, sizeof(Operation), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);


    /*
    int status = startserver();
    if(status != 0){
        qDebug() << "Failed to start server with status:" << status;
        return -1;
    }
    */
    Operation op = *send_operation;

    while (!vcstopFlag.load()) {
        sem_wait(sem);

        if(op == Operation::STOP){
            //come_stop_flag = true;
            //follow_stop_flag = true;
            MoveControll::getInstance().Stop();
            iscomeorfollow = false;
        }
        else {
            if(iscomeorfollow){ //normal operation cannot be executed when come or follow is running
                continue;
            }
            else
            {
                MoveControll::getInstance().SetFromOperation(op);
                if(op == Operation::COME || op == Operation::FOLLOW){
                    iscomeorfollow = false;//nedd to fix
                }
            }
        }
    }

    MoveControll::getInstance().Stop();

    munmap(send_operation, sizeof(int));
    close(shm_fd);
    shm_unlink(SHM_NAME);
    sem_close(sem);
    sem_unlink(SEM_NAME);


    return 0;
}

int VoiceControl::voicecontrol_stop(){
    vcstopFlag.store(true);
    //stopserver();
    return 0;
}   
