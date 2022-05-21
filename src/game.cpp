#include "game.h"
#include <iostream>
#include <memory>
#include "SDL.h"
#include <algorithm>
#include "controller.h"
#include <thread>

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : //snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
    for (size_t ns = 0; ns < 2; ns++)
    {
       snakes.push_back(std::make_shared<Snake>(grid_width, grid_height));
    }
    PlaceFood();
}

Game::~Game()
{
    // set up thread barrier before this object is destroyed
    std::for_each(threads.begin(), threads.end(), [](std::thread &t) {
        t.join();
    });
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

/////////////////////////////////////////////////////////////////////////////////////////  
//  std::for_each(snakes.begin(), snakes.end(), [](std::shared_ptr<Snake> &snake) {
//      threads.emplace_back(std::thread(&Snake::Update, snake));
//  });
  for (size_t ns = 0; ns < 2; ns++) {
      threads.emplace_back(std::thread(&Snake::Update, snakes[ns]));
  };
//  std::promise<bool> prms;
//  std::future<bool> ftr = prms.get_future();
  
  threads.emplace_back(std::thread(&Controller::HandleInput, controller, std::ref(running), std::ref(snakes[0])));
  
//  if(ftr.get() == false) return;
/////////////////////////////////////////////////////////////////////////////////////////  
  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.

    Update();
    renderer.Render(snakes[0], food);
    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score[0], frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.

    if (!snakes[0]->SnakeCell(x, y) && !snakes[1]->SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::Update() {

    if (!snakes[0]->alive && !snakes[1]->alive ) return;

    for(size_t s = 0; s < 2; s++){
      int new_x = static_cast<int>(snakes[s]->head_x);
      int new_y = static_cast<int>(snakes[s]->head_y);

      // Check if there's food over here
      if (food.x == new_x && food.y == new_y) {
        score[s]++;
        PlaceFood();
        // Grow snake and increase speed.
        snakes[s]->GrowBody();
        snakes[s]->speed += 0.02;
        break;
      }
    }
}

int Game::GetScore() const { return score[0]; }
int Game::GetSize() const { return snakes[0]->size; }