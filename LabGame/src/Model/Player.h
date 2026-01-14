#ifndef LABGAME_SRC_MODEL_PLAYER_H_
#define LABGAME_SRC_MODEL_PLAYER_H_
#include "ICreature.h"

class Player : public ICreature {
public:
  std::string GetImageFileName() const override;
  int GetDrawingPriority() const override { return 0; }

  ObjectCommand Act(int x, int y) override;
  bool DeadInConflict(ObjectPtr conflictedObject) override;

private:
  ObjectCommand TryMove(int newX, int newY, int deltaX, int deltaY);
};

#endif  // LABGAME_SRC_MODEL_PLAYER_H_
