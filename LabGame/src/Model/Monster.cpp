#include "Monster.h"
#include "GameState.h"
#include "Player.h"
#include "Objects.h"
#include "CreatureFactory.h"

namespace {
    std::shared_ptr<IGameObject> CreateMonster() {
        return std::make_shared<Monster>();
    }
    bool registered = CreatureFactory::Instance().Register('M', CreateMonster);
}

float Monster::timeUntilDig = 10.0f;
const float Monster::DIG_INTERVAL = 10.0f;
bool Monster::isDiggerActive = false;

// --- Monster ---

bool Monster::DeadInConflict(ObjectPtr conflictedObject) {
  if (std::dynamic_pointer_cast<Monster>(conflictedObject) != nullptr ||
    std::dynamic_pointer_cast<MonsterDigger>(conflictedObject) != nullptr) {
    return false;
  }

  bool dies = (std::dynamic_pointer_cast<Sack>(conflictedObject) != nullptr ||
         std::dynamic_pointer_cast<Fire>(conflictedObject) != nullptr);

  if (dies && std::dynamic_pointer_cast<Fire>(conflictedObject) != nullptr) {
    GameState::Scores += 50;
  }
  return dies;
}

ObjectCommand Monster::TryMoveMonster(int targetX, int targetY, int deltaX, int deltaY) {
  if (GameState::IsOutOfBounds(targetX, targetY)) return {};

  auto target = GameState::Map[targetX][targetY];
  if (target && (std::dynamic_pointer_cast<Terrain>(target) ||
         std::dynamic_pointer_cast<Sack>(target) ||
         std::dynamic_pointer_cast<Monster>(target) ||
         std::dynamic_pointer_cast<MonsterDigger>(target) ||
         std::dynamic_pointer_cast<Fire>(target))) {
    return {};
  }
  return {deltaX, deltaY, nullptr};
}

ObjectCommand Monster::BaseAct(int x, int y) {
  if (GameState::IsOver) return {};

  auto playerPos = GameState::GetPlayerPosition();
  if (playerPos.first == -1) return {};

  int px = playerPos.first;
  int py = playerPos.second;

  if (px != x) {
    int deltaX = (px > x) ? 1 : -1;
    auto cmd = TryMoveMonster(x + deltaX, y, deltaX, 0);
    if (cmd.deltaX != 0 || cmd.deltaY != 0) return cmd;
  }

  if (py != y) {
    int deltaY = (py > y) ? 1 : -1;
    auto cmd = TryMoveMonster(x, y + deltaY, 0, deltaY);
    if (cmd.deltaX != 0 || cmd.deltaY != 0) return cmd;
  }
  return {};
}

ObjectCommand Monster::Act(int x, int y) {
  return BaseAct(x, y);
}

// --- MonsterDigger ---

bool MonsterDigger::DeadInConflict(ObjectPtr conflictedObject) {
  if (std::dynamic_pointer_cast<Terrain>(conflictedObject) != nullptr) {
    return false;
  }

  if (std::dynamic_pointer_cast<Sack>(conflictedObject) != nullptr) {
    return false;
  }
  return Monster::DeadInConflict(conflictedObject);
}

ObjectCommand MonsterDigger::Act(int x, int y) {
  digTime -= TIME_PER_TICK;

  if (digTime <= 0.0f) {
    Monster::isDiggerActive = false;
    Monster::timeUntilDig = Monster::DIG_INTERVAL;

    ObjectPtr newCreature = std::make_shared<Monster>();
    return ObjectCommand{0, 0, newCreature};
  }

  auto playerPos = GameState::GetPlayerPosition();
  int deltaX = 0;
  int deltaY = 0;

  if (playerPos.first != -1) {
    int px = playerPos.first;
    int py = playerPos.second;

    if (px != x) {
      deltaX = (px > x) ? 1 : -1;
    } else if (py != y) {
      deltaY = (py > y) ? 1 : -1;
    }
  }

  if (deltaX != 0 || deltaY != 0) {
    int targetX = x + deltaX;
    int targetY = y + deltaY;

    if (GameState::IsOutOfBounds(targetX, targetY)) return {};

    auto target = GameState::Map[targetX][targetY];

    if (target && std::dynamic_pointer_cast<Terrain>(target) == nullptr &&
     (std::dynamic_pointer_cast<Monster>(target) ||
      std::dynamic_pointer_cast<MonsterDigger>(target) ||
      std::dynamic_pointer_cast<Fire>(target))) {
      return {};
    }

    return {deltaX, deltaY, nullptr};
  }

  return {};
}
