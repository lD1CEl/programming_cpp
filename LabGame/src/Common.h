#ifndef LABGAME_SRC_COMMON_H_
#define LABGAME_SRC_COMMON_H_
#include <memory>
#include <string>

class IGameObject;
using ObjectPtr = std::shared_ptr<IGameObject>;
// Compatibility alias/transition
using CreaturePtr = ObjectPtr; 

const int TILE_SIZE = 32;
constexpr float SCREEN_SCALE = 1.0f;
const float TIME_PER_TICK = 0.15f;

struct ObjectCommand {
  int deltaX = 0;
  int deltaY = 0;
  ObjectPtr transformTo = nullptr;
};
using CreatureCommand = ObjectCommand;


#endif  // LABGAME_SRC_COMMON_H_
