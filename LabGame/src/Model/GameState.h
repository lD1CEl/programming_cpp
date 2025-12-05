#pragma once
#include <vector>
#include "../Common.h"
#include <SFML/Window/Keyboard.hpp>

class GameState {
public:
  static int MapWidth;
  static int MapHeight;
  static std::vector<std::vector<CreaturePtr>> Map;
  static int Scores;
  static bool IsOver;
  static sf::Keyboard::Key KeyPressed;

  static bool IsFiring;
  static float FireTimer;
  static int PlayerDirectionX;
  static int PlayerDirectionY;

  static bool IsWin;
  static bool IsLost;

  static bool IsOutOfBounds(int x, int y);
};
