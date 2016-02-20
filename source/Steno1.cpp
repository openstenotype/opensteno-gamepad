#include <SDL2/SDL.h>
#include <iostream>

char text [26];

int main(int argc, char *argv[])
{
    for (int i=0; i < 25; i++) {
        text[i]=' ';
    }
    text[0]='|';
    text[25]='|';
    
    SDL_Init(SDL_INIT_EVERYTHING);
    
    SDL_bool done = SDL_FALSE;
    int counter = 0;
    
    SDL_Window *window;
    window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 300, 400, SDL_WINDOW_RESIZABLE);
    
    SDL_StartTextInput();
    
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
        /* Hab ich eine Taste gedrückt?*/
            switch (event.type) {
                case SDL_QUIT:
                    done = SDL_TRUE;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.repeat == 0) {
                        counter++;
                        //printf("Key %s pressed detected (%d)\n", SDL_GetScancodeName(event.key.keysym.scancode), counter);
                        /* Welche Taste habe ich auf der Tastatur gedrückt?
                         * http://www.zav.cz/german/compkomasch.htm
                         */
                         switch (event.key.keysym.scancode) {
                         case SDL_SCANCODE_ESCAPE:
                         done = SDL_TRUE;
                         break;
                         
                         case SDL_SCANCODE_Q:
                         text[1]='S';
                         break;
                         
                         case SDL_SCANCODE_A:
                         text[1]='S';
                         break;
                         
                         case SDL_SCANCODE_W:
                         text[2]='B';
                         break;
                         
                         case SDL_SCANCODE_S:
                         text[3]='T';
                         break;
                         
                         case SDL_SCANCODE_E:
                         text[4]='G';
                         break;
                         
                         case SDL_SCANCODE_D:
                         text[5]='D';
                         break;
                         
                         case SDL_SCANCODE_R:
                         text[6]='R';
                         break;
                         
                         case SDL_SCANCODE_F:
                         text[7]='L';
                         break;
                         
                         case SDL_SCANCODE_C:
                         text[8]='A';
                         break;
                         
                         case SDL_SCANCODE_V:
                         text[9]='U';
                         break;
                         
                         case SDL_SCANCODE_T:
                         case SDL_SCANCODE_G:
                         case SDL_SCANCODE_Y:
                         case SDL_SCANCODE_H:
                         text[10]='*';
                         break;
                         
                         case SDL_SCANCODE_N:
                         text[11]='O';
                         break;
                         
                         case SDL_SCANCODE_M:
                         text[12]='I';
                         break;
                         
                         case SDL_SCANCODE_U:
                         text[13]='R';
                         break;
                         
                         case SDL_SCANCODE_J:
                         text[14]='L';
                         break;
                         
                         case SDL_SCANCODE_I:
                         text[15]='M';
                         break;
                         
                         case SDL_SCANCODE_K:
                         text[16]='B';
                         break;
                         
                         case SDL_SCANCODE_O:
                         text[17]='G';
                         break;
                         
                         case SDL_SCANCODE_L:
                         text[18]='D';
                         break;
                         
                         case SDL_SCANCODE_P:
                         text[19]='S';
                         break;
                         
                         case SDL_SCANCODE_SEMICOLON:
                         text[20]='T';
                         break;
                    
                         case SDL_SCANCODE_LEFTBRACKET:
                         text[21]='N';
                         break;
                                 
                         case SDL_SCANCODE_APOSTROPHE:
                         text[22]='E';
                         break;
                                 
                         
                         default:
                         std::cout << event.key.keysym.scancode;
                         break;
                        
                         }
                         
                    }
                    break;
                
                case SDL_KEYUP:
                    if (event.key.repeat == 0) {
                        counter--;
                        //printf("Key %s release detected (%d)\n", SDL_GetScancodeName(event.key.keysym.scancode), counter);
                        if (counter==0) {
                            text[23] = '\0';
                            printf("Telegramm: ");
                            for (int i=0; i < 26; i++) {
                                printf("%c",text[i]);
                            }
                            printf("\n");
                            
                            for (int i=0; i < 26; i++) {
                                text[i]=' ';
                            }
                            text[0]='|';
                            text[25]='|';
                            
                        }
                    }
                    break;
                
                default:
                    break;
            }
        }
    }
    
    
    return 0;
}
