#ifndef LABGAME_SRC_MODEL_MONSTER_H_
#define LABGAME_SRC_MODEL_MONSTER_H_
#include "ICreature.h"

class Monster : public ICreature {
public:
  static float timeUntilDig;
  static const float DIG_INTERVAL;
  static bool isDiggerActive;

public:
  std::string GetImageFileName() const override { return "images/Monster.png"; }
  int GetDrawingPriority() const override { return 1; }

  bool DeadInConflict(CreaturePtr conflictedObject) override;
  CreatureCommand Act(int x, int y) override;

protected:
  CreatureCommand BaseAct(int x, int y);
  CreatureCommand TryMoveMonster(int targetX, int targetY, int deltaX, int deltaY);
  std::pair<int, int> FindPlayer();
};

class MonsterDigger : public Monster {
private:
  float digTime = 5.0f;

public:
  std::string GetImageFileName() const override { return "images/monster_digger.png"; }
  int GetDrawingPriority() const override { return 1; }

  bool DeadInConflict(CreaturePtr conflictedObject) override;
  CreatureCommand Act(int x, int y) override;
};

#endif  // LABGAME_SRC_MODEL_MONSTER_H_
