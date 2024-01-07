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

std::vector<Bord> Bords;
/*
    1. Separation
    2. Alignment
    3. Cohesion
*/
void enactGameRules () {
    std::cout << "rules are rules" << std::endl;
}

void stepAll () {
    for (Bord& b : Bords) {
        b.step();
    }
}

void drawPieces (SDL_Renderer *&renderer) {
    for (Bord& b : Bords) {
        b.drawBord(renderer);
        // std::cout << b.getDirVector()[0] << " " << b.getDirVector()[1] << std::endl;
    }
}

void removeBordOnCursor (int (&mouse) [2]) {
    for (Bord& b : Bords) {
        if (b.isPointInBord(mouse)) {
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

    for (size_t i = 0; i < BLOCKSONSCREENHEIGHT; i++)
    {
        p1[Y] = p2[Y] = i*BLOCKHEIGHT;

        SDL_RenderDrawLine(renderer, p1[X], p1[Y], p2[X], p2[Y]);
    }
    

    // ------ draw cols -----------
    p1[Y] = 0;
    p2[Y] = SCREENHEIGHT;
    
    for (size_t i = 0; i < BLOCKSONSCREENWIDTH; i++)
    {
        p1[X] = p2[X] = i*BLOCKWIDTH;

        SDL_RenderDrawLine(renderer, p1[X], p1[Y], p2[X], p2[Y]);
    }
}

int main (int argc, char* argv[]) {
    
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(SCREENWIDTH, SCREENHEIGHT, 0, &window, &renderer);   

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
    // needs to be a ptr if we want to use it for everyone while we pass references
    Bord* currBord = new Bord(vector, mouse);

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
                        
                        std::cout << "moving vector" << std::endl;
                        SDL_GetMouseState(&currX, &currY);
                        vector[0] = currX - currBord->getCenter()[0];
                        vector[1] = currY - currBord->getCenter()[1];
                        // std::cout << Bords[0].getDirVector()[0] << " " << Bords[0].getDirVector()[1] << std::endl;
                        // std::cout << vector[0] << " " << vector[1] << std::endl;
                        currBord->setDirVector(vector);
                        // std::cout << Bords[0].getDirVector()[0] << " " << Bords[0].getDirVector()[1] << std::endl;

                        SDL_SetRenderDrawColor(renderer,255,100,50, 255);
                        SDL_RenderDrawLine(renderer, currBord->getCenter()[0], currBord->getCenter()[1],
                        currX, currY);
                    }
                    std::cout << "lclick " << lClick << std::endl;
                    std::cout << "rclick " << rClick << std::endl;
                    break;
                case SDL_MOUSEBUTTONDOWN: {
                        
                        // it only does this once
                        // when mouse is pressed, mouse pos is {0,0}
                        // clickState holds which mouse button was pressed
                        clickState = SDL_GetMouseState(&(mouse[0]), &(mouse[1]));
                        std::cout << clickState << std::endl;
                        // this ternary if is used to not override prev click, e.g. holding left and clicking right

                        // assuming you can't click both at the same time,
                        // such that first you click one and then the other
                        
                        //allows for lclick and rclick to both be on, and not overwrite eachother
                        lClick = !lClick ? clickState == LCLICK || clickState == LRCLICK : lClick;
                        rClick = !rClick ? clickState == RCLICK || clickState == LRCLICK : rClick;
                        // lClick = clickState == LCLICK;
                        // rClick = clickState == RCLICK;
                        if (lClick && !rClick) {
                            std::cout << "left";
                            vector[0] = mouse[0]; vector[1] = mouse[1];
                            currBord->setCenter(mouse);
                            Bords.push_back(*currBord);
                        } if (rClick) {
                            // rClick only works on pause
                            std::cout << "right";
                        }
                        std::cout << "down" << std::endl;
                        hold = (lClick || rClick);  
                        // pushback performs a move operator, such that there's no copy involved.
                        
                    }
                    break;
                case SDL_MOUSEBUTTONUP: {
                    int mouseState = SDL_GetMouseState(NULL, NULL);
                    std::cout << "true 1" << std::endl;
                    std::cout << mouseState << std::endl;
                    std::cout << lClick << std::endl;
                    
                    // i don't want other mouse buttons to interfere
                    if (mouseState == RCLICK || mouseState == LCLICK || lClick || rClick) {
                        std::cout << "true 2" << std::endl;
                        // if rClick is being held right now
                        if (mouseState == RCLICK || lClick) {
                            std::cout << "left up" << std::endl;
                            // same vector as defined by mouse
                            lClick = false;
                            // both buttons can be down at the same time
                            // if (!rClick && SDL_GetMouseState(NULL, NULL) == RCLICK) {std::cout << "hold false" << std::endl; hold = false;}
                            // i hope these don't change inside bord
                            vector[0] = vector[1] = 0;
                            // as the vector is of objects passed by ref,
                            // we shouldn't delete the ptr before restarting it.
                            // Bords are automatically deleted
                            // we restart currBord
                            currBord = new Bord(vector, mouse);

                        } else if (mouseState == LCLICK || rClick) {
                            std::cout << "right up" << std::endl;
                            rClick = false;
                            // both buttons can be down at the same time
                            // if (!lClick && SDL_GetMouseState(NULL, NULL) == LCLICK) {std::cout << "hold false" << std::endl; hold = false;}
                        } 
                    }
                }
                    break;
                case SDL_KEYDOWN:
                    state = SDL_GetKeyboardState(nullptr);
                    if (state) {
                        pause ^= state[SDL_SCANCODE_SPACE] || state[SDL_SCANCODE_P];
                    }

                    // pause is true if either space or p are pressed.
                    // if pause was already pressed then stop
                    // if it wasn't then start. This is represented by XOR

                    break;
                case SDL_KEYUP:
                    state = nullptr;
                    break;
            }
        }
        
        

        // put pieces, theyll have a random
        // favourably you can tune the speed vector, strech it and turn it 
        if (pause && (lClick || rClick)) {

            // this one is outside keyevent, such that we can delete a lot by holding
            // if not both are clicked
            if (rClick && !lClick) {
                removeBordOnCursor(mouse);
            }
        }

        if (!pause) {
            // tickTime is in seconds, and Sleep gets MS
            Sleep(tickTime*1000);
            enactGameRules();
            stepAll();
        }
        
        
        // draw grid on screen
        SDL_Color gridColor{70,70,40,255};
        drawGrid(renderer, gridColor);

        drawPieces(renderer);

        // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        // SDL_RenderDrawLine(renderer, mouse[0], mouse[1], currX, currY);

        // it is not shown backwards, all changes to render are buffered
        SDL_RenderPresent(renderer);

        
        // if (!pause) {
        //     SDL_SetRenderDrawColor(renderer,0,0,0,255);
        //     SDL_RenderClear(renderer);
        // }
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}