#ifndef LABGAME_SRC_COMMON_H_
#define LABGAME_SRC_COMMON_H_
#include <memory>
#include <string>

class ICreature;
using CreaturePtr = std::shared_ptr<ICreature>;

const int TILE_SIZE = 32;
constexpr float SCREEN_SCALE = 1.0f;
const float TIME_PER_TICK = 0.15f;

struct CreatureCommand {
  int deltaX = 0;
  int deltaY = 0;
  CreaturePtr transformTo = nullptr;
};

#endif  // LABGAME_SRC_COMMON_H_
