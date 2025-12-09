#include "Objects.h"
#include "GameState.h"
#include "Player.h"
#include "Monster.h"
#include "CreatureFactory.h"

namespace {
    std::shared_ptr<ICreature> CreateTerrain() {
        return std::make_shared<Terrain>();
    }
    bool registeredTerrain = CreatureFactory::Instance().Register('T', CreateTerrain);

    std::shared_ptr<ICreature> CreateGold() {
        return std::make_shared<Gold>();
    }
    bool registeredGold = CreatureFactory::Instance().Register('G', CreateGold);

    std::shared_ptr<ICreature> CreateSack() {
        return std::make_shared<Sack>();
    }
    bool registeredSack = CreatureFactory::Instance().Register('S', CreateSack);
}

// --- Terrain ---
bool Terrain::DeadInConflict(CreaturePtr conflictedObject) {
  return (std::dynamic_pointer_cast<Player>(conflictedObject) != nullptr ||
      std::dynamic_pointer_cast<MonsterDigger>(conflictedObject) != nullptr);
}

// --- Gold ---
bool Gold::DeadInConflict(CreaturePtr conflictedObject) {
  if (std::dynamic_pointer_cast<Player>(conflictedObject)) {
    GameState::Scores += 10;
  }
  return (std::dynamic_pointer_cast<Player>(conflictedObject) != nullptr ||
      std::dynamic_pointer_cast<MonsterDigger>(conflictedObject) != nullptr);
}

// --- Sack ---
bool Sack::DeadInConflict(CreaturePtr conflictedObject) {
  return (std::dynamic_pointer_cast<MonsterDigger>(conflictedObject) != nullptr);
}

CreatureCommand Sack::Act(int x, int y) {
  if (y + 1 >= GameState::MapHeight) {
    if (fallDistance > 1) {
      fallDistance = 0;
      return {0, 0, std::make_shared<Gold>()};
    }
    fallDistance = 0;
    return {};
  }
  auto below = GameState::Map[x][y + 1];

  if (below == nullptr) {
    fallDistance++;
    return {0, 1, nullptr};
  }

  if (std::dynamic_pointer_cast<Player>(below) ||
    std::dynamic_pointer_cast<Monster>(below) ||
    std::dynamic_pointer_cast<MonsterDigger>(below))
  {
    if (fallDistance >= 1) {
      if (std::dynamic_pointer_cast<Monster>(below) ||
        std::dynamic_pointer_cast<MonsterDigger>(below)) {
        GameState::Scores += 100;
      }

      fallDistance++;
      return {0, 1, nullptr};
    }

    fallDistance = 0;
    return {};
  }

  if (fallDistance > 1 && (std::dynamic_pointer_cast<Terrain>(below) ||
              std::dynamic_pointer_cast<Sack>(below) ||
              std::dynamic_pointer_cast<Gold>(below))) {
    fallDistance = 0;
    return {0, 0, std::make_shared<Gold>()};
  }

  fallDistance = 0;
  return {};
}

// --- Fire ---
bool Fire::DeadInConflict(CreaturePtr conflictedObject) {
  return conflictedObject != nullptr && std::dynamic_pointer_cast<Player>(conflictedObject) == nullptr;
}
