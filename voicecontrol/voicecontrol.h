#ifndef VOICECONTROL_H
#define VOICECONTROL_H

#include <QProcess>

#define sever_path = "/home/group3/model/vociecontrol"
#define SHM_NAME "/shm_vc"
#define SEM_NAME "/sem_vc"


class VoiceControl : public QObject {
    Q_OBJECT

public:
    VoiceControl() : vcstopFlag(false), server(new QProcess(this)) {}

public slots:
    int voicecontrol_start();
    int voicecontrol_stop();

private:
    std::atomic<bool> vcstopFlag;
    int startserver();
    int stopserver();
    QProcess* server;
};

#endif // VOICECONTROL_H
