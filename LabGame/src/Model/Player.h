#pragma once
#include "ICreature.h"

class Player : public ICreature {
public:
  std::string GetImageFileName() const override;
  int GetDrawingPriority() const override { return 0; }

  CreatureCommand Act(int x, int y) override;
  bool DeadInConflict(CreaturePtr conflictedObject) override;

private:
  CreatureCommand TryMove(int newX, int newY, int deltaX, int deltaY);
};
