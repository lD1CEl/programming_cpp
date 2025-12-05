#ifndef LABGAME_SRC_CONTROLLER_GAMECONTROLLER_H_
#define LABGAME_SRC_CONTROLLER_GAMECONTROLLER_H_
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

#endif  // LABGAME_SRC_CONTROLLER_GAMECONTROLLER_H_
