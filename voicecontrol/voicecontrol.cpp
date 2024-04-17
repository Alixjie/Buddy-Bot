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

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        return 1;
    }
    Operation* recv_operation = (Operation*)mmap(0, sizeof(Operation), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t* sem = sem_open(SEM_NAME, 0);


    /*
    int status = startserver();
    if(status != 0){
        qDebug() << "Failed to start server with status:" << status;
        return -1;
    }
    */
    Operation op;
    while (!vcstopFlag.load()) {
        sem_wait(sem);
        op = *recv_operation;
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
                printf("operation:%d\n",(int)op);
                MoveControll::getInstance().SetFromOperation(op);
                if(op == Operation::COME || op == Operation::FOLLOW){
                    iscomeorfollow = false;//nedd to fix
                }
            }
        }
    }

    MoveControll::getInstance().Stop();

    munmap(recv_operation, sizeof(Operation));
    close(shm_fd);
    sem_close(sem);


    return 0;
}

int VoiceControl::voicecontrol_stop(){
    vcstopFlag.store(true);
    //stopserver();
    return 0;
}   
