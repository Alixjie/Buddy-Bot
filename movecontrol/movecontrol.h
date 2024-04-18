

#ifndef CARCONTROLLER_H
#define CARCONTROLLER_H
#include "follow/ulm3_samples.h"
#include "utils/semaphore.h"

enum class Operation {
  COME,
  LEFT,
  RIGHT,
  STOP,
  FOLLOW,
  GO,
  NONE // no operation
};

// class MainWindow;
// class ULM3Samples;

class MoveControl {
public:
  /*
  enum MoveState {
      STOP,
      FORWARD,
      SPIN,
      BACKWARD
  };
  */

  static MoveControl &getInstance();

  MoveControl(const MoveControl &) = delete;
  MoveControl &operator=(const MoveControl &) = delete;

  // MainWindow* getMainWindow() const { return mainWindow; }
  // void setMainWindow(MainWindow *mainWindow) { this->mainWindow = mainWindow;
  // }

  int getMovestate() const { return movestate; }
  int getDistance() const { return Distance; }
  int getAngle() const { return angle; }
  void setMovestate(int state) { movestate = state; }

  int SetFromOperation(Operation operation);
  int SetFromAngel(int angle);
  int SetFromDistance(int distance);

  Semaphore sem;
  void Stop();

  static MoveControl *instance;

private:
  int movestate;
  int Distance;
  int angle;

  // MainWindow *mainWindow;

  std::mutex m_mutex;

  MoveControl() : movestate(0), Distance(0), angle(0), sem(false) {}

  void setDistance(unsigned int distance) { Distance = distance; }

  void setAngle(int angle) { this->angle = angle; }

  void SetMoveState(int movestate, int Distance, int anlge);
};

#endif // CARCONTROLLER_H
