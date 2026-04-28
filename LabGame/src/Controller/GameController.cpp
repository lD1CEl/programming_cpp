#include "GameController.h"
#include "../View/GameView.h"
#include "../Model/GameLogic.h"
#include "../Model/GameState.h"
#include "../Common.h"
#include <SFML/Window/Event.hpp>

GameController::GameController() {
  RestartGame();
  view = std::make_unique<GameView>();
}

GameController::~GameController() = default;

void GameController::Run() {
  while (view->IsOpen()) {
    HandleEvents();
    Update();
    
    view->Clear();
    view->Render();
    view->Display();
  }
}

void GameController::HandleEvents() {
  sf::Event event;
  while (view->PollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      view->Close();
    }

    if (GameState::IsOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
      RestartGame();
      view->PlayMusic();
    }
  }
}

void GameController::Update() {
  float deltaTime = globalClock.restart().asSeconds();

  if (GameState::FireTimer > 0.0f) {
    GameState::FireTimer -= deltaTime;
    if (GameState::FireTimer < 0.0f) {
      GameState::FireTimer = 0.0f;
    }
  }

  if (tickClock.getElapsedTime().asSeconds() > TIME_PER_TICK && !GameState::IsOver) {
    ExecuteTick();
    tickClock.restart();
  }
  
  if (GameState::IsOver) {
    view->PauseMusic();
  }
}
