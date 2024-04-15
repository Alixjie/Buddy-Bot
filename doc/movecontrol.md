# MoveControl  modules

## Overview
This middleware layer serves as an intermediary between the following and obstacle avoidance systems, and the motor control module. It receives motion commands from the following and obstacle avoidance systems, converts these commands, and then forwards them to the motor control module. Additionally, it implements a semaphore to acquire the next segment of the path after the vehicle completes a current segment.

## Interfaces

### Upper Interface
- int MoveControll::SetFromAngel(int angle):	use to spin
- int MoveControll::SetFromDistance(int distance)  use to forward
- int MoveControll::SetFromOperation(Operation operation) used by voice control module
  - operation: COME,LEFT,RIGHT,STOP,FOLLOW,GO


### Lower Interface
- void MoveControll::SetMoveState(int movestate , int Distance, int anlge)
  - movestate: 0 stop, 1 forward , 2 spin

