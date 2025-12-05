#pragma once
#include <memory>
#include <SFML/System/Clock.hpp>

class GameView;

class GameController {
public:
  GameController();
  ~GameController();
  void Run();

private:
  std::unique_ptr<GameView> view;
  sf::Clock tickClock;
  sf::Clock globalClock;

  void HandleEvents();
  void Update();
};
