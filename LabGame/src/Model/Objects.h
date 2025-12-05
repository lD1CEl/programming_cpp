#pragma once
#include "ICreature.h"

class Terrain : public ICreature {
public:
  std::string GetImageFileName() const override { return "images/Terrain.png"; }
  int GetDrawingPriority() const override { return 10; }
  CreatureCommand Act(int, int) override { return {}; }
  bool DeadInConflict(CreaturePtr conflictedObject) override;
};

class Gold : public ICreature {
public:
  std::string GetImageFileName() const override { return "images/Gold.png"; }
  int GetDrawingPriority() const override { return 5; }
  CreatureCommand Act(int, int) override { return {}; }
  bool DeadInConflict(CreaturePtr conflictedObject) override;
};

class Sack : public ICreature {
private:
  int fallDistance = 0;

public:
  std::string GetImageFileName() const override { return "images/Sack.png"; }
  int GetDrawingPriority() const override { return 2; }
  CreatureCommand Act(int x, int y) override;
  bool DeadInConflict(CreaturePtr conflictedObject) override;
};

class Fire : public ICreature {
public:
  int deltaX, deltaY;
  Fire(int dx, int dy) : deltaX(dx), deltaY(dy) {}
  std::string GetImageFileName() const override { return "images/fire.png"; }
  int GetDrawingPriority() const override { return 8; }
  CreatureCommand Act(int, int) override { return {deltaX, deltaY, nullptr}; }
  bool DeadInConflict(CreaturePtr conflictedObject) override;
};
