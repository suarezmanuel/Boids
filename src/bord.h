#include <SDL2/SDL.h>
#include <algorithm>
#include <iterator>
#include <iostream>
#define BORDWIDTH 20
#define BORDHEIGHT 20
#define RADIUS 60

class Bord {
    SDL_Rect box;
    int vector [2] = {0};
    int center [2] = {0};
    int r = RADIUS;

public:
    Bord (int (&vector) [2], int (&center) [2]);
    void drawBord (SDL_Renderer *&renderer);
    void step ();
    bool isPointInBord (int (&point) [2]);
    int* getVector ();
    void setVector (int (&vector) [2]);
    int* getCenter ();
    void setCenter (int (&center) [2]);
    // operator implemented for std::find to work (used in main.cpp)
    bool operator== (const Bord& other) const;
    // needs to be a non-member, as the class puts 
    friend std::ostream& operator<< (std::ostream& os, const Bord& b);
};