#ifndef GAME_H
#define GAME_H

#include <random>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include <memory>
#include <thread>

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  ~Game();
 
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
 
 protected:
  std::vector<std::thread> threads; // holds all threads that have been launched within this object

 private:
  std::vector<std::shared_ptr<Snake>> snakes;
  SDL_Point food;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  std::vector<int> score{0,0};
  
  void PlaceFood();
  void Update();
};

#endif