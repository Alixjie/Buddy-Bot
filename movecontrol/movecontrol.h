

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

    /*
    enum MoveState {
        STOP,
        FORWARD,
        SPIN,
        BACKWARD
    };
    */

    static MoveControll& getInstance();

    MoveControll(const MoveControll&) = delete;
    MoveControll& operator=(const MoveControll&) = delete;

    int getMovestate() const { return movestate; }
    int getDistance() const { return Distance; }
    int getAngle() const { return angle; }
    void setMovestate(int state) { movestate = state; }

    int SetFromOperation(Operation operation);
    int SetFromAngel(int angle);
    int SetFromDistance(int distance);

    Semaphore sem;

    static MoveControll *instance;


private:
    int movestate;
    int Distance;
    int angle;


    std::mutex m_mutex;

    MoveControll() :movestate(0), Distance(0), angle(0), sem(false) {
    }


    void setDistance(unsigned int distance) { Distance = distance; }


    void setAngle(int angle) { this->angle = angle; }

    void Stop();

    void SetMoveState(int movestate , int Distance, int anlge);
    

};

#endif // CARCONTROLLER_H
