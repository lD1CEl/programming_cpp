#include "GameLogic.h"
#include "GameState.h"
#include "Objects.h"
#include "Player.h"
#include "Monster.h"
#include "CreatureFactory.h"
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>

void CreateMapFromString(const std::string& mapStr) {
  std::vector<std::string> rows;
  std::stringstream ss(mapStr);
  std::string segment;

  while (std::getline(ss, segment, '\n')) {
    if (!segment.empty() && segment.back() == '\r') { segment.pop_back(); }
    if (!segment.empty()) { rows.push_back(segment); }
  }

  if (rows.empty()) return;

  GameState::MapHeight = (int)rows.size();

  size_t maxWidth = 0;
  for (const auto& row : rows) {
    if (row.size() > maxWidth) {
      maxWidth = row.size();
    }
  }
  GameState::MapWidth = (int)maxWidth;
  GameState::Map.resize(GameState::MapWidth, std::vector<ObjectPtr>(GameState::MapHeight, nullptr));

  for (int y = 0; y < GameState::MapHeight; y++) {
    const std::string& row = rows[y];
    for (int x = 0; x < (int)row.size(); x++) {
      char c = row[x];

      ObjectPtr creature = nullptr;
      try {
          if (c != ' ') {
             creature = CreatureFactory::Instance().CreateObject(c);
          }
      } catch (const DefaultFactoryError<char, std::shared_ptr<IGameObject>>::Exception& e) {
          std::cerr << "Error: Unknown creature type '" << e.GetId() << "' at map coordinates (" << x << ", " << y << ")" << std::endl;
          creature = nullptr;
      }

      if (std::dynamic_pointer_cast<Player>(creature)) {
           if (GameState::PlayerDirectionX == 0 && GameState::PlayerDirectionY == 0) GameState::PlayerDirectionX = 1;
      }

      GameState::Map[x][y] = creature;
    }
  }
}

const std::string initialMapStr =
  "TTTTTTTTTTTTTTTTTTTTTTTTTTTTSTTT\n"
  "T GTTTTTTTTTTTTTTTTTTTTTTTTTTTTT\n"
  "TTTTTTTTTTTTTTSTTTSTTTTTTTTTTT M\n"
  "TTTTSTTTTTTTTTTTTTTTTTTTSTTTTT T\n"
  "TTTTTTTTTTTTTTTTM  TTTTTT TTTT T\n"
  "TTTTTTSTTTTSTTTTTTTTSTTTM TTT  T\n"
  "TTSM TTTTTTTTTTTTTTTTTTTTT    TT\n"
  "TTT  TTTTTTTT  TTT   P     TTTTT\n"
  "TTTTTTM STTTTTTT  TTTTTTTTTTTTTT\n"
  "TTTTTT  TTTTT    TTTTTTTTTTTTTTT\n"
  "TTTTTTTTT     TTTTTTTTTTTTTTTTTT\n"
  "TT        TTTTTTTTTTTTTTTTTTTTTT\n"
  "   TTTTTTTTTTTTTTTTTTTTTTTTTTTTT\n"
  "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT\n"
  "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT\n"
  "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT\n"
  "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT\n"
  "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT";

void RestartGame() {
  GameState::Scores = 0;
  GameState::IsOver = false;
  GameState::IsWin = false;
  GameState::IsLost = false;
  GameState::IsFiring = false;
  GameState::FireTimer = 0.0f;
  Monster::isDiggerActive = false;
  Monster::timeUntilDig = Monster::DIG_INTERVAL;

  std::srand(static_cast<unsigned int>(std::time(0)));

  CreateMapFromString(initialMapStr);
}

struct CreatureAction {
  CreatureCommand Command;
  CreaturePtr Creature;
  int X, Y;
  int TargetX, TargetY;
};

void ExecuteTick() {
  if (GameState::IsOver) return;

  if (Monster::isDiggerActive == false) {
    if (Monster::timeUntilDig > 0.0f) {
      Monster::timeUntilDig -= TIME_PER_TICK;
    } else {
      std::vector<std::pair<int, int>> livingMonsters;
      for (int x = 0; x < GameState::MapWidth; x++) {
        for (int y = 0; y < GameState::MapHeight; y++) {
          if (GameState::Map[x][y] && std::dynamic_pointer_cast<Monster>(GameState::Map[x][y])) {
            livingMonsters.push_back({x, y});
          }
        }
      }

      if (!livingMonsters.empty()) {
        int randomIndex = std::rand() % livingMonsters.size();
        auto [randX, randY] = livingMonsters[randomIndex];

        GameState::Map[randX][randY] = std::make_shared<MonsterDigger>();
        Monster::isDiggerActive = true;
      } else {
        Monster::timeUntilDig = Monster::DIG_INTERVAL;
      }
    }
  }

  std::vector<CreatureAction> rawActions;
  for (int x = 0; x < GameState::MapWidth; x++) {
    for (int y = 0; y < GameState::MapHeight; y++) {
      auto creature = GameState::Map[x][y];
      if (creature) {
        auto cmd = creature->Act(x, y);

        if (cmd.deltaX != 0 || cmd.deltaY != 0 || cmd.transformTo != nullptr) {
          if (x + cmd.deltaX >= 0 && x + cmd.deltaX < GameState::MapWidth &&
            y + cmd.deltaY >= 0 && y + cmd.deltaY < GameState::MapHeight)
          {
            rawActions.push_back({cmd, creature, x, y, x + cmd.deltaX, y + cmd.deltaY});
          }
        }
      }
    }
  }

  std::vector<std::vector<CreaturePtr>> mapReservations(GameState::MapWidth, std::vector<CreaturePtr>(GameState::MapHeight, nullptr));
  std::vector<CreatureAction> confirmedActions;

  for (const auto& action : rawActions) {
    int targetX = action.TargetX;
    int targetY = action.TargetY;

    bool isMoving = action.Command.deltaX != 0 || action.Command.deltaY != 0;
    bool isHighPriority = std::dynamic_pointer_cast<Monster>(action.Creature) ||
               std::dynamic_pointer_cast<Player>(action.Creature) ||
               std::dynamic_pointer_cast<MonsterDigger>(action.Creature);

    bool actionConfirmed = true;

    if (isMoving && isHighPriority && action.Command.transformTo == nullptr) {

      auto currentReservation = mapReservations[targetX][targetY];

      if (currentReservation != nullptr) {
        int currentPri = action.Creature->GetDrawingPriority();
        int resPri = currentReservation->GetDrawingPriority();

        if (currentPri < resPri) {
          mapReservations[targetX][targetY] = action.Creature;
        } else if (currentPri > resPri) {
          actionConfirmed = false;
        } else {
          actionConfirmed = false;
        }
      } else {
        mapReservations[targetX][targetY] = action.Creature;
      }
    }

    if (actionConfirmed) {
      confirmedActions.push_back(action);
    }
  }

  std::vector<std::vector<std::vector<CreaturePtr>>> nextMapCandidates(
    GameState::MapWidth, std::vector<std::vector<CreaturePtr>>(GameState::MapHeight));

  std::vector<std::vector<bool>> cellIsSource(GameState::MapWidth, std::vector<bool>(GameState::MapHeight, false));

  for (const auto& action : confirmedActions) {
    auto nextCreature = action.Command.transformTo ? action.Command.transformTo : action.Creature;

    if (action.Command.deltaX != 0 || action.Command.deltaY != 0 || action.Command.transformTo != nullptr) {
      cellIsSource[action.X][action.Y] = true;
    }

    if (nextCreature) {
      nextMapCandidates[action.TargetX][action.TargetY].push_back(nextCreature);
    }
  }

  // --- FIRE SPAWN ---
  if (GameState::IsFiring) {
    GameState::IsFiring = false;

    int startX = -1, startY = -1;
    for (int i = 0; i < GameState::MapWidth; i++) {
      for (int j = 0; j < GameState::MapHeight; j++) {
        if (GameState::Map[i][j] && std::dynamic_pointer_cast<Player>(GameState::Map[i][j])) {
          startX = i; startY = j;
          break;
        }
      }
    }

    if (startX != -1) {
      int targetX = startX + GameState::PlayerDirectionX;
      int targetY = startY + GameState::PlayerDirectionY;

      if (!GameState::IsOutOfBounds(targetX, targetY)) {
        auto currentTarget = GameState::Map[targetX][targetY];
        if (currentTarget == nullptr ||
          std::dynamic_pointer_cast<Gold>(currentTarget) ||
          std::dynamic_pointer_cast<Monster>(currentTarget) ||
          std::dynamic_pointer_cast<MonsterDigger>(currentTarget))
        {
          nextMapCandidates[targetX][targetY].push_back(
            std::make_shared<Fire>(GameState::PlayerDirectionX, GameState::PlayerDirectionY)
          );
        }
      }
    }
  }

  for (int x = 0; x < GameState::MapWidth; x++) {
    for (int y = 0; y < GameState::MapHeight; y++) {
      auto creature = GameState::Map[x][y];
      if (!creature) continue;

      if (!cellIsSource[x][y]) {
        nextMapCandidates[x][y].push_back(creature);
      }
    }
  }

  std::vector<std::vector<CreaturePtr>> newMap(
    GameState::MapWidth, std::vector<CreaturePtr>(GameState::MapHeight, nullptr));

  for (int x = 0; x < GameState::MapWidth; x++) {
    for (int y = 0; y < GameState::MapHeight; y++) {
      auto& candidates = nextMapCandidates[x][y];
      if (candidates.empty()) continue;

      std::sort(candidates.begin(), candidates.end());
      candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());

      std::vector<CreaturePtr> alive = candidates;

      for (const auto& candidate : candidates) {
        bool isDead = false;
        for (const auto& rival : candidates) {
          if (rival != candidate && candidate->DeadInConflict(rival)) {
            isDead = true;
            break;
          }
        }
        if (isDead) {
          if (std::dynamic_pointer_cast<MonsterDigger>(candidate)) {
            Monster::isDiggerActive = false;
            Monster::timeUntilDig = Monster::DIG_INTERVAL;
          }

          alive.erase(std::remove(alive.begin(), alive.end(), candidate), alive.end());

          if (std::dynamic_pointer_cast<Player>(candidate)) {
            GameState::IsOver = true;
            GameState::IsLost = true;
          }
        }
      }

      std::sort(alive.begin(), alive.end(), [](const CreaturePtr& a, const CreaturePtr& b){
        return a->GetDrawingPriority() < b->GetDrawingPriority();
      });
      alive.erase(std::unique(alive.begin(), alive.end()), alive.end());


      if (alive.empty()) {
        newMap[x][y] = nullptr;
      } else {
        newMap[x][y] = alive[0];
      }
    }
  }

  GameState::Map = newMap;

  // --- WIN CONDITION ---
  if (!GameState::IsOver) {
    int valuableCount = 0;
    for (int x = 0; x < GameState::MapWidth; x++) {
      for (int y = 0; y < GameState::MapHeight; y++) {
        if (GameState::Map[x][y]) {
          if (std::dynamic_pointer_cast<Gold>(GameState::Map[x][y]) ||
            std::dynamic_pointer_cast<Sack>(GameState::Map[x][y])) {
            valuableCount++;
          }
        }
      }
    }

    if (valuableCount == 0) {
      GameState::IsWin = true;
      GameState::IsOver = true;
    }
  }
}
