#include <SDL2/SDL.h>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>
#include <math.h>
#define BORDWIDTH 5
#define BORDHEIGHT 5
#define MINPROX 1
#define RADIUS 60
#define REDUCTIONFACTOR 4

class Bord {
    SDL_Rect box;
    int vector [2] = {0};
    int center [2] = {0};
    int r = RADIUS;
    SDL_Color color;

public:
    Bord (int (&vector) [2], int (&center) [2]);
    void drawBord (SDL_Renderer *&renderer);
    void step ();
    bool isPointInBord (int (&point) [2]);
    int* getVector ();
    void setVector (int (&vector) [2]);
    void setVector (int angle, float size);
    void setColor (SDL_Color color);
    int* getCenter ();
    void setCenter (int (&center) [2]);
    float getVectorSize ();
    int getVectorAngle ();
    // float averageVectorSize (std::vector<Bord*>& neighbors);
    // int averageVectorAngle (std::vector<Bord*>& neighbors);
    // int* averageCenter (std::vector<Bord*>& neighbors);
    // bool checkVeryCloseNeighbor (std::vector<Bord*>& Bords);
    // std::vector<Bord*> getNeighbors (std::vector<Bord*>& Bords);
    float getDistanceFromBord (Bord *other);
    // void separation (std::vector<Bord*>& Bords);
    // void cohesion (std::vector<Bord*>& Bords);
    // void alignment (std::vector<Bord*>& Bords);
    float getPointsDistance (int *a, int *b);
    // operator implemented for std::find to work (used in main.cpp)
    bool operator== (const Bord& other) const;
    // needs to be a non-member, as the class puts 
    friend std::ostream& operator<< (std::ostream& os, const Bord& b);
};