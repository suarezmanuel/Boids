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

    enum clicks {
        LCLICK=1, MCLICK, IDK, RCLICK
    };

    while (running) {

        if (SDL_PollEvent(&windowEvent)) {
            
            switch (windowEvent.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEMOTION:
                    if (hold && lClick) {
                        std::cout << "moving vector" << std::endl;
                        SDL_GetMouseState(&currX, &currY);
                        vector[0] = currX - mouse[0];
                        vector[1] = currY - mouse[1];
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // it only does this once
                    // when mouse is pressed, mouse pos is {0,0}
                    // clickState holds which mouse button was pressed
                    clickState = SDL_GetMouseState(&(mouse[0]), &(mouse[1]));
                    lClick = (clickState == LCLICK);
                    rClick = (clickState == RCLICK);
                    hold = (lClick || rClick);  
                    vector[0] = mouse[0]; vector[1] = mouse[1];
                    break;
                case SDL_MOUSEBUTTONUP:
                    std::cout << "up" << std::endl;
                    lClick = rClick = hold = false;
                    break;
                case SDL_KEYDOWN:
                    state = SDL_GetKeyboardState(nullptr);
                    if (state) {
                        pause ^= state[SDL_SCANCODE_SPACE] || state[SDL_SCANCODE_P];
                    }

                    // pause is true if either space or p are pressed.
                    // if pause was already pressed then stop
                    // if it wasn't then start. This is represented by XOR

                    // must wait atleast 0.1 seconds between pauses
                    break;
                case SDL_KEYUP:
                    state = nullptr;
                    break;
            }
        }
        
        

        // put pieces, theyll have a random
        // favourably you can tune the speed vector, strech it and turn it 
        if (pause && (lClick || rClick)) {

            // std::cout << Bords.size() << std::endl;

            if (lClick) {
                // same vector as defined by mouse
                Bord bord(vector, mouse); 
                Bords.push_back(bord);
            }

            if (rClick) {
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

        if (!pause) {
            SDL_SetRenderDrawColor(renderer,0,0,0,255);
            SDL_RenderClear(renderer);
        }
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}