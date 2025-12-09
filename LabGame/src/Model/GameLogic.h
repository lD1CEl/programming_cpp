#ifndef LABGAME_SRC_MODEL_GAMELOGIC_H_
#define LABGAME_SRC_MODEL_GAMELOGIC_H_
#include <string>

void CreateMapFromString(const std::string& mapStr);
void RestartGame();
void ExecuteTick();

#endif  // LABGAME_SRC_MODEL_GAMELOGIC_H_
