 /*****************************************************************************

                                                         Author: Jason Ma
                                                         Date:   Sep 25 2016
                                      SDLGame

 File Name:     Display.cpp
 Description:   SDL interface, handles events and displays everything
                appropriately.
 *****************************************************************************/

#include "SDL2/include/SDL2/SDL.h"
#include "SDL_image/include/SDL2/SDL_image.h"
#include "Display.h"
//#include <png.h>
//#include <zlib.h>
#include <iostream>
#include <cstdio>

using std::cout;
using std::endl;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Window * window = NULL; //window to render to
SDL_Surface * screenSurface = NULL; //surface contained by window
SDL_Renderer * renderer = NULL;
SDL_Texture * mouseTexture;
SDL_Event event;
SDL_Rect mouseRect;
SDL_Rect tile;

int Display::init() {
  //attempt to init SDL
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL init failed! SDL_Error: %s\n", SDL_GetError());
    return -1;
  }
 
  //attempt to create window  
  window = SDL_CreateWindow("Cubeception 3 Status Monitor", 
      SDL_WINDOWPOS_UNDEFINED, 
      SDL_WINDOWPOS_UNDEFINED, 
      SCREEN_WIDTH, 
      SCREEN_HEIGHT, 
      SDL_WINDOW_SHOWN);

  if(window == NULL) {
    printf("Window creation failed! SDL_Error: %s\n", SDL_GetError());
    return -1;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if(renderer == NULL) {
    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    return -1;
  }

  int imgFlags = IMG_INIT_PNG;

  if(!(IMG_Init(imgFlags) & imgFlags)) {
    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
  }

  screenSurface = SDL_GetWindowSurface(window);

  SDL_FillRect(screenSurface, NULL, 
      SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));

  SDL_UpdateWindowSurface(window);

  SDL_ShowCursor(SDL_DISABLE);

  return 0;
}

int Display::load() {
  mouseTexture = loadTexture("crosshair16.png");
  //TODO actually check whether texture was loaded, unloaded -> NULL
  return 0;
}

SDL_Texture * Display::loadTexture(std::string path) {
  SDL_Texture * result = NULL;

  SDL_Surface * loadedSurface = IMG_Load(path.c_str());
  if(loadedSurface == NULL) {
    printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    return result;
  }

  result = SDL_CreateTextureFromSurface(renderer, loadedSurface);

  if(result == NULL) {
    printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    return result;
  }

  SDL_FreeSurface(loadedSurface);

  return result;
}

int Display::handleEvents() {
  int x, y;

  while(SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_QUIT:
        return -1;
      case SDL_KEYDOWN:
        switch(event.key.keysym.sym) {
          case SDLK_q:
            return -1;
        }
        break;
      case SDL_MOUSEMOTION:

        //get mouse position, draw rect
        SDL_GetMouseState(&x, &y);
        mouseRect = {x - 8, y - 8, 16, 16};

        //cout << x << " " << y << endl;
    }
  }
  return 0;
}

void Display::render() {
  //clear screen
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(renderer);

  //draw rect
  /*
  SDL_Rect rect = {400, 400, 400, 400};
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderFillRect(renderer, &rect);
  */

  SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
  
  //render tiles
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8; j++) {
      tile.x = i * 50;
      tile.y = j * 50;
      tile.w = 49;
      tile.h = 49;
      SDL_RenderFillRect(renderer, &tile);
    }

  //render crosshair
  SDL_RenderCopy(renderer, mouseTexture, NULL, &mouseRect);

  //show frame
  SDL_RenderPresent(renderer);

  //cout << x << " " << y << endl;
}

void Display::stop() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main(int argc, char * argv[]) {
  Display disp;
  if(disp.init()) {
    cout << "Init Error" << endl;
    return -1;
  }

  if(disp.load()) {
    cout << "Load Error" << endl;
    return -1;
  }

  int quit = 0;
  int frames = 0;
  mouseRect = {0, 0, 10, 10};
  unsigned int begin = SDL_GetTicks();
  while(!quit) {
    if(SDL_GetTicks() - begin > 1000) {
      begin = SDL_GetTicks();
      cout << "FPS: " << frames << endl;
      frames = 0;
    }
    if(disp.handleEvents() == -1)
      break;
    disp.render();
    frames++;
  }
  disp.stop();

  return 0; 
}


