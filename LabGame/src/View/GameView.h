#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "TextureManager.h"
#include "../Model/GameState.h"

class GameView {
public:
  GameView();
  bool IsOpen() const;
  bool PollEvent(sf::Event& event);
  void Close();
  void Clear();
  void Render();
  void Display();
  void PlayMusic();
  void PauseMusic();

private:
  sf::RenderWindow window;
  TextureManager texManager;
  sf::Font font;
  sf::Text scoreText;
  sf::Sprite winScreen;
  sf::Sprite lostScreen;
  sf::Music music;
  bool musicLoaded;
  bool fontLoaded;

  void LoadResources();
  void DrawMap();
  void DrawUI();
  void DrawScreens();
};
