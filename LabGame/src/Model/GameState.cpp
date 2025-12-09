#include "GameState.h"

int GameState::MapWidth = 0;
int GameState::MapHeight = 0;
std::vector<std::vector<CreaturePtr>> GameState::Map;
int GameState::Scores = 0;
bool GameState::IsOver = false;
sf::Keyboard::Key GameState::KeyPressed = sf::Keyboard::Key::Unknown;
bool GameState::IsFiring = false;
float GameState::FireTimer = 0.0f;
int GameState::PlayerDirectionX = 1;
int GameState::PlayerDirectionY = 0;
bool GameState::IsWin = false;
bool GameState::IsLost = false;

bool GameState::IsOutOfBounds(int x, int y) {
  return x < 0 || x >= MapWidth || y < 0 || y >= MapHeight;
}
