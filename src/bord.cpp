#include "bord.h"
#include "draw.h"

// this weird arg sintax allows for getting arg as array and not as ptr
Bord::Bord (int (&dirVector) [2], int (&center) [2]) {

    setDirVector(dirVector);
    setCenter(center);
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
    // we need to add the center as it stems from the center
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

int* Bord::getDirVector () {
    return dirVector;
}

void Bord::setDirVector (int (&dirVector) [2]) {
    std::copy(std::begin(dirVector), std::end(dirVector), std::begin(this->dirVector));
    // dirVector[0] = 0; dirVector[1] = 0;
    // std::cout << dirVector[0] << " " << dirVector[1] << std::endl;
    // std::cout << this->dirVector[0] << " " << this->dirVector[1] << std::endl;
    // this->dirVector[0] = dirVector[0];
    // this->dirVector[1] = dirVector[1];
    // std::cout << "vector updated " << dirVector[0] << " " << dirVector[1] << std::endl;
}

int* Bord::getCenter () {
    int *ret = center;
    return ret;
}

void Bord::setCenter (int (&center) [2]) {
    //sets point
    this->center[0] = center[0];
    this->center[1] = center[1];

    //sets rectangle
    box.h = BORDHEIGHT; box.w = BORDWIDTH;
    box.x = center[0] - box.w/2; box.y = center[1] - box.h/2;
}

bool Bord::operator== (const Bord& other) const {
    // comparing height, width as maybe in the far future nature may allow different sized birds
    return box.h == other.box.h && box.w == other.box.w && box.x == other.box.x && box.y == other.box.y
    &&     dirVector[0] == other.dirVector[0] && dirVector[1] == other.dirVector[1]
    &&     center[0] == other.center[0] && center[1] == other.center[1]
    &&     r == other.r;
}

//get vec size
