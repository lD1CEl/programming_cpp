#include "Player.h"
#include "GameState.h"
#include "Objects.h"
#include "Monster.h"
#include <SFML/Window/Keyboard.hpp>

std::string Player::GetImageFileName() const {
  if (GameState::FireTimer > 0.0f) {
    return "images/Digger_after_fire.png";
  }
  return "images/Digger.png";
}

CreatureCommand Player::Act(int x, int y) {
  int deltaX = 0;
  int deltaY = 0;

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) deltaX = -1;
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) deltaX = 1;
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) deltaY = -1;
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) deltaY = 1;

  if (deltaX != 0 || deltaY != 0) {
    GameState::PlayerDirectionX = deltaX;
    GameState::PlayerDirectionY = deltaY;
  } else if (GameState::PlayerDirectionX == 0 && GameState::PlayerDirectionY == 0) {
    GameState::PlayerDirectionX = 1;
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
    if (GameState::FireTimer <= 0.0f) {
      GameState::FireTimer = 10.0f;
      GameState::IsFiring = true;
    }
    return {};
  }

  if (deltaX != 0 || deltaY != 0) {
    return TryMove(x + deltaX, y + deltaY, deltaX, deltaY);
  }

  return {};
}

bool Player::DeadInConflict(CreaturePtr conflictedObject) {
  bool dies = (std::dynamic_pointer_cast<Sack>(conflictedObject) != nullptr ||
      std::dynamic_pointer_cast<Monster>(conflictedObject) != nullptr ||
      std::dynamic_pointer_cast<MonsterDigger>(conflictedObject) != nullptr);

  if (std::dynamic_pointer_cast<Fire>(conflictedObject) != nullptr) {
    return false;
  }

  if (dies) {
    GameState::IsOver = true;
    GameState::IsLost = true;
  }
  return dies;
}

CreatureCommand Player::TryMove(int newX, int newY, int deltaX, int deltaY) {
  if (GameState::IsOutOfBounds(newX, newY)) return {};
  auto target = GameState::Map[newX][newY];

  if (target && (std::dynamic_pointer_cast<Sack>(target) ||
         std::dynamic_pointer_cast<Monster>(target) ||
         std::dynamic_pointer_cast<MonsterDigger>(target) ||
         std::dynamic_pointer_cast<Fire>(target))) {
    return {};
  }

  return {deltaX, deltaY, nullptr};
}
