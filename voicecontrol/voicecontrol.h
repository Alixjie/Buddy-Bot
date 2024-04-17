#ifndef VOICECONTROL_H
#define VOICECONTROL_H

#include <QProcess>
#include <thread>

#define sever_path = "/home/group3/model/vociecontrol"
#define SHM_NAME "/shm_vc"
#define SEM_NAME "/sem_vc"


class VoiceControl : public QObject {
    Q_OBJECT

public:
    VoiceControl() : vcstopFlag(true), server(new QProcess(this)) {}

public slots:
    int voicecontrol_start();
    int voicecontrol_stop();

private:
    std::atomic<bool> vcstopFlag;
    int running;
    std::thread voiceThread;
    void voicecontrol_run();
    int startserver();
    int stopserver();
    QProcess* server;
};

#endif // VOICECONTROL_H
