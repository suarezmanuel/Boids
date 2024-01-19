#include "bord.h"
#include "draw.h"

// this weird arg sintax allows for getting arg as array and not as ptr
Bord::Bord (int (&vector) [2], int (&center) [2]) {

    setVector(vector);
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
    center[0] + vector[0], center[1] + vector[1]);
}

// we don't want the vector drawn on the screen to be exactly the step each time,
// we would want it to be less than that (half in here).
void Bord::step () {
    center[0] += vector[0] / REDUCTIONFACTOR;
    center[1] += vector[1] / REDUCTIONFACTOR;

    box.x += vector[0] / REDUCTIONFACTOR;
    box.y += vector[1] / REDUCTIONFACTOR;
}

bool Bord::isPointInBord (int (&point) [2]) {
    return (box.x <= point[0] && point[0] <= box.x + box.w
    &&      box.y <= point[1] && point[1] <= box.y + box.h);
}

int* Bord::getVector () {
    return vector;
}

void Bord::setVector (int (&vector) [2]) {
    std::copy(std::begin(vector), std::end(vector), std::begin(this->vector));
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

void Bord::setColor (SDL_Color c) {
    this->color = c;
}

bool Bord::operator== (const Bord& other) const {
    // comparing height, width as maybe in the far future nature may allow different sized birds
    return box.h == other.box.h && box.w == other.box.w && box.x == other.box.x && box.y == other.box.y
    &&     vector[0] == other.vector[0] && vector[1] == other.vector[1]
    &&     center[0] == other.center[0] && center[1] == other.center[1]
    &&     r == other.r;
}

// TODO: print all info about bord
std::ostream& operator<< (std::ostream& os, Bord const & b) {
    os << "bord center : " << b.center[0] << ", " << b.center[1] << std::endl;
    os << "bord vector : " << b.vector[0] << ", " << b.vector[1] << std::endl;
    return os;
}

float Bord::getPointsDistance (int *a, int *b) {
    return sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2));
}

float Bord::getDistanceFromBord (Bord *other) {
    return getPointsDistance(this->center, other->center);
}

float toRadians (float degree) {
    return degree * M_PI / 180;
}

float toDegrees (float radian) {
    return radian * 180 / M_PI;
}

//get vec size
float Bord::getVectorSize () {
    return getPointsDistance(center, vector);
}

int Bord::getVectorAngle () {
    if (center[0]-vector[0] == 0) {
        return 360;
    }
    float res = atan(toRadians((center[1]-vector[1]) / abs(center[0]-vector[0])));
    return (int) toDegrees(res);
}

void Bord::setVector (int angle, float size) {
    int angleAsRadians = toRadians(angle);
    int y = size * toDegrees(sin(angleAsRadians));
    int x = size * toDegrees(cos(angleAsRadians));
}