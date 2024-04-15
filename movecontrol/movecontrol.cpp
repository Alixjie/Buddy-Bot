// movecontrol.cpp
#include "movecontrol.h"
#include <stdlib.h>


void MoveControll::SetMoveState(int movestate , int Distance, int anlge) {
    std::lock_guard<std::mutex> lock(m_mutex);
    setMovestate(movestate);
    setDistance(Distance);
    setAngle(anlge);
}


void MoveControll::Stop() {
    SetMoveState(0, 0, 0);
}


int MoveControll::SetFromOperation(Operation operation) {
    switch (operation) {
        case Operation::COME:
        // may leave to another control mode here
            break;
        case Operation::LEFT:
            SetFromAngel(-90);
            break;
        case Operation::RIGHT:
            SetFromAngel(90);
            break;
        case Operation::STOP:
            Stop();
            break;
        case Operation::FOLLOW:
        // may leave to another control mode here
            break;
        case Operation::GO:
            SetFromDistance(100);
            break;

        default:
            return -1;  // Invalid operation
    }
    return 0;  // Success
}

//wheelDistance = angle*10/3 + 0.004 angle*angle
//abs angle should between 15 and 90, if not, the result will be not accurate
int MoveControll::SetFromAngel(int angle) {
    if (angle < -90 || angle > 90) {
        return -1;  // Invalid angle
    }
        SetMoveState(2, 0, angle);
    return 0;  // Success
}

//distance is in cm,1cm = 18 wheelDistance, should be larger than 10cm
int MoveControll::SetFromDistance(int distance) {
    if (distance < 0) {
        return -1;  // Invalid distance
    }
    SetMoveState(1, distance, 0);
    return 0;  // Success
}

