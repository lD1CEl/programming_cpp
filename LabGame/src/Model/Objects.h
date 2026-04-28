#ifndef LABGAME_SRC_MODEL_OBJECTS_H_
#define LABGAME_SRC_MODEL_OBJECTS_H_
#include "IGameObject.h"

class Terrain : public IGameObject {
public:
  std::string GetImageFileName() const override { return "images/Terrain.png"; }
  int GetDrawingPriority() const override { return 10; }
  ObjectCommand Act(int, int) override { return {}; }
  bool DeadInConflict(ObjectPtr conflictedObject) override;
};

class Gold : public IGameObject {
public:
  std::string GetImageFileName() const override { return "images/Gold.png"; }
  int GetDrawingPriority() const override { return 5; }
  ObjectCommand Act(int, int) override { return {}; }
  bool DeadInConflict(ObjectPtr conflictedObject) override;
};

class Sack : public IGameObject {
private:
  int fallDistance = 0;

public:
  std::string GetImageFileName() const override { return "images/Sack.png"; }
  int GetDrawingPriority() const override { return 2; }
  ObjectCommand Act(int x, int y) override;
  bool DeadInConflict(ObjectPtr conflictedObject) override;
};

class Fire : public IGameObject {
public:
  int deltaX, deltaY;
  Fire(int dx, int dy) : deltaX(dx), deltaY(dy) {}
  std::string GetImageFileName() const override { return "images/fire.png"; }
  int GetDrawingPriority() const override { return 8; }
  ObjectCommand Act(int, int) override { return {deltaX, deltaY, nullptr}; }
  bool DeadInConflict(ObjectPtr conflictedObject) override;
};

#endif  // LABGAME_SRC_MODEL_OBJECTS_H_
