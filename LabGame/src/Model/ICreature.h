#pragma once
#include "../Common.h"
#include <string>

class ICreature {
public:
  virtual ~ICreature() = default;
  virtual std::string GetImageFileName() const = 0;
  virtual int GetDrawingPriority() const = 0;
  virtual CreatureCommand Act(int x, int y) = 0;
  virtual bool DeadInConflict(CreaturePtr conflictedObject) = 0;
};
