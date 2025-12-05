#ifndef LABGAME_SRC_MODEL_ICREATURE_H_
#define LABGAME_SRC_MODEL_ICREATURE_H_
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

#endif  // LABGAME_SRC_MODEL_ICREATURE_H_
