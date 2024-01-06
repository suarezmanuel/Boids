#include "bord.h"
#include "draw.h"

// this weird arg sintax allows for getting arg as array and not as ptr
Bord::Bord (int (&dirVector) [2], int (&center) [2]) {

    this->dirVector[0] = dirVector[0]; this->dirVector[1] = dirVector[1];
    this->center[0] = center[0]; this->center[1] = center[1];

    // set bord box (drawn as white in game)
    box.h = BORDHEIGHT; box.w = BORDWIDTH;
    box.x = center[0] - box.w/2; box.y = center[1] - box.h/2;
}

void Bord::drawBord (SDL_Renderer *&renderer) {
      
    // draw Bord's FOV
    SDL_SetRenderDrawColor(renderer, 250, 240, 90, 0.01);
    drawCircle(renderer, center[0], center[1], r);

    // draw Bord
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &box);

    // draw Bord's vector
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, center[0], center[1], 
    center[0] + dirVector[0], center[1] + dirVector[1]);
}

void Bord::step () {
    center[0] += dirVector[0];
    center[1] += dirVector[1];

    box.x += dirVector[0];
    box.y += dirVector[1];
}

bool Bord::isPointInBord (int (&point) [2]) {
    return (box.x <= point[0] && point[0] <= box.x + box.w
    &&      box.y <= point[1] && point[1] <= box.y + box.h);
}

bool Bord::operator== (const Bord& other) const {
    // comparing height, width as maybe in the far future nature may allow different sized birds
    return box.h == other.box.h && box.w == other.box.w && box.x == other.box.x && box.y == other.box.y
    &&     dirVector[0] == other.dirVector[0] && dirVector[1] == other.dirVector[1]
    &&     center[0] == other.center[0] && center[1] == other.center[1]
    &&     r == other.r;
}
