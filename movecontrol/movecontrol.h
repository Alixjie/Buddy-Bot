

#ifndef CARCONTROLLER_H
#define CARCONTROLLER_H
#include "../utils/semaphore.h"

enum class Operation {
    COME,
    LEFT,
    RIGHT,
    STOP,
    FOLLOW,
    GO,
    NONE  //no operation
};




class MoveControll { 
public:
    enum MoveState {
        STOP,
        FORWARD,
        SPIN,
        BACKWARD
    };

    static MoveControll& getInstance() {
        static MoveControll instance;
        return instance;
    }


    MoveControll(const MoveControll&) = delete;
    void operator=(const MoveControll&) = delete;

    int SetFromOperation(Operation operation);
    int SetFromAngel(int angle);
    int SetFromDistance(int distance);

    Semaphore sem;


private:
    MoveState movestate;
    unsigned int Distance;
    int angle;

    std::mutex m_mutex;

     MoveControll() :movestate(STOP), Distance(0), angle(0) { }

    MoveState getMovestate() const { return movestate; }
    void setMovestate(MoveState state) { movestate = state; }

    unsigned int getDistance() const { return Distance; }
    void setDistance(unsigned int distance) { Distance = distance; }

    int getAngle() const { return angle; }
    void setAngle(int angle) { this->angle = angle; }

    void Stop();

    void MoveControll::SetMoveState(MoveState movestate , unsigned int Distance, int anlge);
    

};

#endif // CARCONTROLLER_H