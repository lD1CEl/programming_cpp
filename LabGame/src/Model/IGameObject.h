#ifndef LABGAME_SRC_MODEL_IGAMEOBJECT_H_
#define LABGAME_SRC_MODEL_IGAMEOBJECT_H_
#include "../Common.h"
#include <string>

class IGameObject {
public:
  virtual ~IGameObject() = default;
  virtual std::string GetImageFileName() const = 0;
  virtual int GetDrawingPriority() const = 0;
  virtual ObjectCommand Act(int x, int y) = 0;
  virtual bool DeadInConflict(ObjectPtr conflictedObject) = 0;
};

#endif  // LABGAME_SRC_MODEL_IGAMEOBJECT_H_
