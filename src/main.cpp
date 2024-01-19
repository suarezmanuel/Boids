#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <windows.h>
#include "bord.h"
#define SCREENWIDTH 600
#define SCREENHEIGHT 600
#define BLOCKWIDTH 30
#define BLOCKHEIGHT 30
#define BLOCKSONSCREENWIDTH SCREENWIDTH/BLOCKWIDTH 
#define BLOCKSONSCREENHEIGHT SCREENHEIGHT/BLOCKHEIGHT
#define INTSIZE sizeof(int)

// we need references as pushback creates copies
std::vector<Bord*> Bords;
std::vector<Bord*> BordsCopy;





std::vector<Bord*> getNeighbors (Bord *& b, std::vector<Bord*>& Bords) {
    std::vector<Bord*> neighbors;
    for (Bord*& bb : Bords) {
        // an open set. We can be neighbors with ourselves, makes transitions smoother
        if (bb->getDistanceFromBord(b) < RADIUS) {
            std::cout << "new neighbor" << std::endl;
            neighbors.push_back(bb);
        }
    }
    return neighbors;
}



float averageVectorSize (std::vector<Bord*>& neighbors) {
    if (neighbors.size() == 0) {
        return 0;
    }

    float sumSizes = 0;
    for (Bord*& b : neighbors) {
        if (b) {
            sumSizes += b->getVectorSize();
        }
    }
    return sumSizes / neighbors.size();
}



int averageVectorAngle (std::vector<Bord*>& neighbors) {
    if (neighbors.size() == 0) {
        return 0;
    }

    float sumAngles = 0;
    for (Bord*& b : neighbors) {
        if (b) {
            sumAngles += b->getVectorAngle();
        }
    }
    
    return sumAngles / neighbors.size();
}




int* averageCenter (std::vector<Bord*>& neighbors) {
    if (neighbors.size() == 0) {
        return 0;
    }

    int centerSum [2] = {0, 0}, *center = nullptr;

    for (Bord*& b : neighbors) {
        if (b) {
            center = b->getCenter();
            centerSum[0] += center[0];
            centerSum[1] += center[1];
        }
    }

    int* avgCenter  = (int*) malloc (2 * sizeof(int));
    avgCenter[0] = centerSum[0] / neighbors.size();
    avgCenter[1] = centerSum[1] / neighbors.size();

    return avgCenter;
}



bool checkVeryCloseNeighbor (Bord*& b, std::vector<Bord*>& Bords) {
    std::vector<Bord*> neighbors = getNeighbors(b, Bords);
    for (Bord*& b : Bords) {
        // an open set
        if (b->getDistanceFromBord(b) < MINPROX) {
            return true;
        }
    }
    return false;
}




void separation (Bord*& b, std::vector<Bord*>& neighbors) {

    // get avg of vectors to neighbors' centers, and negate it
    int vectorSum [2] = {0, 0}, vectorToCenter[2];
    
    // if theres a very close neighbor
    if (checkVeryCloseNeighbor(b, neighbors)) {
        for (Bord*& bb : neighbors) {
            // here we don't want ourselves as neighbors
            if (bb && bb != b) {
                // vec to center of neighbor
                vectorToCenter[0] = b->getCenter()[0] - bb->getCenter()[0];
                vectorToCenter[1] = b->getCenter()[1] - bb->getCenter()[1];

                vectorSum[0] += vectorToCenter[0];
                vectorSum[1] += vectorToCenter[1];
            }
        }
    }
    
    int avgVector [2] = {0,0};
    avgVector[0] = vectorSum[0] / neighbors.size();
    avgVector[1] = vectorSum[1] / neighbors.size();

    // we don't a ref of this temp val we only want its value before its deletion
    int separationVec [2] = {0,0};
    separationVec[0] = -avgVector[0];
    separationVec[1] = -avgVector[1];
    b->setVector(separationVec);
}




void cohesion (Bord*& b, std::vector<Bord*>& neighbors) {

    int *avgCenter = averageCenter(neighbors);
    int vectorToCenter[2] = {0, 0};
    vectorToCenter[0] = avgCenter[0] - b->getCenter()[0]; 
    vectorToCenter[1] = avgCenter[1] - b->getCenter()[1]; 
    b->setVector(vectorToCenter);
    free(avgCenter);
}

void alignment (Bord*& b, std::vector<Bord*>& neighbors) {
    std::cout << "inside alignment" << std::endl;
    SDL_Color color = {255,0,0,255};
    b->setColor (color);
    // setVector(averageVectorAngle(neighbors), averageVectorSize(neighbors));
    // std::cout << "after change " << std::endl;
}







/*
    1. Separation
    2. Alignment
    3. Cohesion
*/
void enactGameRules () {
    // were gonna need two Bord Boards
    // std::copy (Bords.begin(), Bords.end(), BordsCopy.begin());
    // BordsCopy = Bords;
    std::vector<Bord*> neighbors;
    for (Bord*& b : Bords) {
        if (neighbors = getNeighbors(b, Bords); neighbors.size() > 0) {
            // b->cohesion(neighbors);
            // b->separation(neighbors);
            // std::cout << *b << std::endl;
            alignment(b, neighbors);
        }
    }
    // Bords = BordsCopy;
    // std::copy (BordsCopy.begin(), BordsCopy.end(), Bords.begin());
}

void stepAll () {
    for (Bord*& b : Bords) {
        b->step();
    }
}

void drawPieces (SDL_Renderer *&renderer) {
    for (Bord*& b : Bords) {
        b->drawBord(renderer);
    }
}

void removeBordOnCursor (int (&mouse) [2]) {
    for (Bord*& b : Bords) {
        if (b->isPointInBord(mouse)) {
            // get iterator at b's index, in Bords
            auto iterator = std::find(Bords.begin(), Bords.end(), b);
            // remove b from Bords
            Bords.erase(iterator);
        }
    }
}

void drawGrid (SDL_Renderer *renderer, SDL_Color color) {

    enum Coords {
        X, Y
    };

    int p1 [2] = {0,0};
    int p2 [2] = {0,0};

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // ------ draw rows -----------
    p1[X] = 0;
    p2[X] = SCREENWIDTH;

    for (int i = 0; i < BLOCKSONSCREENHEIGHT; i++)
    {
        p1[Y] = p2[Y] = i*BLOCKHEIGHT;

        SDL_RenderDrawLine(renderer, p1[X], p1[Y], p2[X], p2[Y]);
    }
    

    // ------ draw cols -----------
    p1[Y] = 0;
    p2[Y] = SCREENHEIGHT;
    
    for (int i = 0; i < BLOCKSONSCREENWIDTH; i++)
    {
        p1[X] = p2[X] = i*BLOCKWIDTH;

        SDL_RenderDrawLine(renderer, p1[X], p1[Y], p2[X], p2[Y]);
    }
}

// returns 1 for width oob, 2 for height oob, 3 for both.
int checkBordOutOfBounds (Bord* b) {
    if (!b) {return false;}

    int *center = b->getCenter();
    int oobW = (center[0] > SCREENWIDTH || center[0] < 0) ? 1 : 0;
    int oobH = (center[1] > SCREENHEIGHT || center[1] < 0) ? 2 : 0;
    return oobW + oobH;
}

void wrapAroundAll () {
    int *vector;
    int *center;
    int newCenter[2];
    int oob;
    // bad idea, they are null
    // newCenter[0] = center[0];
    // newCenter[1] = center[1];
    for (Bord* b : Bords) {
        if (b && (oob = checkBordOutOfBounds(b))) {
            vector = b->getVector();
            center = b->getCenter();

            newCenter[0] = center[0];
            newCenter[1] = center[1];
            // this is all supposing that b is moving in any direction
            // too much right
            if (oob == 1 || oob == 3) {
                if (vector[0] > 0) {
                    newCenter[0] = 0;
                } else {
                    newCenter[0] = SCREENWIDTH;
                }
            }
            
            if (oob == 2 || oob == 3) {
                if (vector[1] > 0) {
                    newCenter[1] = 0;
                } else {
                    newCenter[1] = SCREENHEIGHT;
                }
            }
            
            b->setCenter(newCenter);
        }
    }
}

int main (int argc, char* argv[]) {
    
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(SCREENWIDTH, SCREENHEIGHT, SDL_RENDERER_PRESENTVSYNC, &window, &renderer);   

    if (window == NULL) {
        std::cout << "Could not create window: " << SDL_GetError() << std::endl;
    }

    bool running = true, lClick = false, rClick = false, pause = true, hold = false;
    int mouse [2] = {0};
    int vector [2] = {0};
    SDL_Event windowEvent;
    SDL_KeyboardEvent keyEvent;

    // a ptr to a const Uint8, has an array of all the states that now hold
    const Uint8 *state;
    int clickState;
    float tickTime = 0.02;
    int currX = 0, currY = 0;
    int prev = 0;
    bool renderCurrLine = false;
    // needs to be a ptr if we want to use it for everyone while we pass references
    Bord* currBord = nullptr;

    enum clicks {
        LCLICK=1, MCLICK, IDK, RCLICK, LRCLICK
    };


    while (running) {
        
        // draw black background
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        if (SDL_PollEvent(&windowEvent)) {
            
            switch (windowEvent.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEMOTION:

                    if (hold && lClick) {
                        // if we created a bord
                        if (currBord) {
                            // we only need the values when moving
                            SDL_GetMouseState(&mouse[0], &mouse[1]);
                            vector[0] = mouse[0] - currBord->getCenter()[0];
                            vector[1] = mouse[1] - currBord->getCenter()[1];
                            currBord->setVector(vector);
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN: {
                        
                        clickState = SDL_GetMouseState(&(mouse[0]), &(mouse[1]));
                        // std::cout << "click down pos: " << mouse[0] << ", " << mouse[1] << std::endl;
                        //allows for lclick and rclick to both be on, and not overwrite eachother
                        lClick = !lClick ? clickState == LCLICK || clickState == LRCLICK : lClick;
                        rClick = !rClick ? clickState == RCLICK || clickState == LRCLICK : rClick;
                        hold = (lClick || rClick);
                        
                        if (pause && lClick && !rClick) {

                            vector[0] = 0; vector[1] = 0;
                            currBord = new Bord(vector, mouse);

                            if (currBord) {
                                currBord->setCenter(mouse);
                                // pushback performs copy
                                Bords.push_back(currBord);
                            }
                        } if (rClick) { }
                    }
                    break;
                case SDL_MOUSEBUTTONUP: {

                    // will tell us whos still holdin
                    int mouseState = SDL_GetMouseState(NULL, NULL);
                    bool lMakeFalse = false;
                    bool rMakeFalse = false;

                    // only left is down or left up while right down
                    if ((lClick && !rClick) || (mouseState == RCLICK)) {
                        lMakeFalse = true;
                    }

                    if ((lClick && !rClick)) {
                        
                        // i hope these don't change inside bord
                        vector[0] = vector[1] = 0;
                        // were done with past bord, waiting for new bord
                        currBord = nullptr;
                    } 
                    
                    if ((rClick && !lClick) || (mouseState == LCLICK)) {
                        rMakeFalse = true;
                    }

                    // couldn't change rClick and lClick before, as their values are used after such changes
                    // only if they aren't false we can "turn them off"
                    lClick = lClick ? !lMakeFalse : lClick;
                    rClick = rClick ? !rMakeFalse : rClick;
                    hold = mouseState == LCLICK || mouseState == RCLICK;
                    }
                    break;
                case SDL_KEYDOWN:
                    state = SDL_GetKeyboardState(nullptr);
                    // pause if p or space, if presssed again stop
                    if (state) {
                        pause ^= state[SDL_SCANCODE_SPACE] || state[SDL_SCANCODE_P];
                    }
                    break;
                case SDL_KEYUP:
                    state = nullptr;
                    break;
            }
        }


        if (!lClick && pause && hold && rClick) {
            SDL_GetMouseState(&(mouse[0]), &(mouse[1]));
            // delete a lot at once
            removeBordOnCursor(mouse);
        } 

        if (!pause) {
            // tickTime is in seconds, and Sleep gets MS
            Sleep(tickTime*1000);
            enactGameRules();
            wrapAroundAll();
            stepAll();
        }
        
        // draw grid on screen
        SDL_Color gridColor{70,70,40,255};
        drawGrid(renderer, gridColor);

        drawPieces(renderer);

        if (mouse[0] == 0 && mouse[1] == 0) {
            SDL_GetMouseState(&(mouse[0]), &(mouse[1]));
        }

        // it is not shown backwards, all changes to render are buffered
        SDL_RenderPresent(renderer);

        mouse[0] = mouse[1] = 0;
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}