#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <SDL_mixer.h>
#include <time.h>


#define SDL_MAIN_HANDLED

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
char appPath[512];
Uint32 lastTime; // Globale variabele voor tijd bijhouden


typedef struct {
    char name[256];
    char logo_path[512];
    char exe_path[512];
    char info_text[1024];
    char credits_text[1024];
    char banner_path[512];
} Game;

Game games[100];
int game_count = 0;
int selected_game = 0;
float anim_progress = 1.0f;
const float anim_speed = .75f;
int direction = 0;
TTF_Font *font = NULL;

typedef enum {
    SCREEN_GAME_SELECT,
    SCREEN_GAME_INFO
} ScreenState;

ScreenState current_screen = SCREEN_GAME_SELECT;

void getAppPath() {
    GetModuleFileName(NULL, appPath, sizeof(appPath));
    char *lastSlash = strrchr(appPath, '\\');
    if (lastSlash) {
        *lastSlash = '\0';  // Verwijdert de bestandsnaam om de directory te krijgen
    }
    printf("App path set to: %s\n", appPath);
}

void initAudio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        printf("SDL_mixer could not initialize: %s\n", Mix_GetError());
    }
}
void playRandomTrack() {
    DIR *dir;
    struct dirent *entry;
    int track_count = 0;
    char *tracks[100];
    char musicPath[512];
    snprintf(musicPath, sizeof(musicPath), "%s\\music", appPath); // Path to the music folder

    if ((dir = opendir(musicPath)) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (strstr(entry->d_name, ".mp3")) {
                tracks[track_count] = malloc(strlen(musicPath) + strlen(entry->d_name) + 2);
                snprintf(tracks[track_count], 512, "%s/%s", musicPath, entry->d_name);
                track_count++;
            }
        }
        closedir(dir);

        if (track_count > 0) {
            srand(time(NULL));
            int randomIndex = rand() % track_count;
            Mix_Music *music = Mix_LoadMUS(tracks[randomIndex]);
            if (music) {
                Mix_VolumeMusic(MIX_MAX_VOLUME / 5); // Set volume to 20%
                printf("Playing: %s\n", tracks[randomIndex]);
                Mix_FadeInMusic(music, 1, 7500); // Fade in over 2 seconds
                SDL_Delay(Mix_PlayingMusic() ? 480000 : 960000); // Play for 8 to 16 minutes
                Mix_FadeOutMusic(7500); // Fade out over 2 seconds
                Mix_FreeMusic(music);
            }
            for (int i = 0; i < track_count; i++) {
                free(tracks[i]);
            }
        } else {
            printf("No MP3 files found in the music directory.\n");
        }
    }
}

Uint32 scheduleTrackPlayback(Uint32 interval, void *param) {
    playRandomTrack();
    return (rand() % (16 - 8 + 1) + 8) * 60000; // Return a random interval between 8 and 16 minutes
}
void setupPaths() {
    char fontPath[512];
    snprintf(fontPath, sizeof(fontPath), "%s\\fonts\\Source_Code_Pro\\static\\SourceCodePro-Regular.ttf", appPath);

    font = TTF_OpenFont(fontPath, 18);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
    } else {
        printf("Font loaded successfully from %s\n", fontPath);
    }
}

void getDesktopPath(char *desktopPath, size_t size) {
    char *homePath = getenv("USERPROFILE");  // Geeft het pad naar de gebruikersmap
    if (homePath) {
        snprintf(desktopPath, size, "%s\\Desktop\\Arcade Games", homePath);
    } else {
        fprintf(stderr, "Could not find user profile path.\n");
    }
}

void loadGames() {
    char arcadeGamesPath[512];
    getDesktopPath(arcadeGamesPath, sizeof(arcadeGamesPath));

    DIR *dir = opendir(arcadeGamesPath);
    if (!dir) {
        fprintf(stderr, "Cannot open Arcade Games folder at %s. Please check the path.\n", arcadeGamesPath);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s\\%s", arcadeGamesPath, entry->d_name);

        struct stat path_stat;
        stat(full_path, &path_stat);

        if (S_ISDIR(path_stat.st_mode) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            Game game;
            snprintf(game.name, sizeof(game.name), "%s", entry->d_name);
            snprintf(game.logo_path, sizeof(game.logo_path), "%s\\%s\\coverart.png", arcadeGamesPath, entry->d_name);

            // Check if coverart.png exists; if not, use CoverArtNotFound.png
            struct stat buffer;
            if (stat(game.logo_path, &buffer) != 0) {
                snprintf(game.logo_path, sizeof(game.logo_path), "%s\\img\\CoverArtNotFound.png", appPath);
            }

            snprintf(game.exe_path, sizeof(game.exe_path), "%s\\%s\\%s.exe", arcadeGamesPath, entry->d_name, entry->d_name);
            snprintf(game.info_text, sizeof(game.info_text), "%s\\%s\\info.txt", arcadeGamesPath, entry->d_name);
            snprintf(game.credits_text, sizeof(game.credits_text), "%s\\%s\\credits.txt", arcadeGamesPath, entry->d_name);
            snprintf(game.banner_path, sizeof(game.banner_path), "%s\\%s\\banner.png", arcadeGamesPath, entry->d_name);

            games[game_count++] = game;
            printf("Loaded game: %s\n", game.name);
        }
    }
    closedir(dir);
}


void renderLogo(SDL_Renderer *renderer) {
    char logoPath[512];
    snprintf(logoPath, sizeof(logoPath), "%s\\img\\noorderpoort.png", appPath); // Dynamisch pad naar logo in de build-map

    SDL_Surface *logoSurface = IMG_Load(logoPath);
    if (logoSurface) {
        SDL_Texture *logoTexture = SDL_CreateTextureFromSurface(renderer, logoSurface);
        SDL_FreeSurface(logoSurface);

        int logoWidth, logoHeight;
        SDL_QueryTexture(logoTexture, NULL, NULL, &logoWidth, &logoHeight);

        SDL_Rect logoRect = {SCREEN_WIDTH / 2 - logoWidth / 2, 20, logoWidth, logoHeight};
        SDL_RenderCopy(renderer, logoTexture, NULL, &logoRect);

        SDL_DestroyTexture(logoTexture);
        printf("Logo loaded from: %s\n", logoPath);
    } else {
        SDL_Log("Failed to load logo from %s: %s", logoPath, IMG_GetError());
    }
}
void renderControls(SDL_Renderer *renderer) {
    char controlsPath[512];
    snprintf(controlsPath, sizeof(controlsPath), "%s\\img\\controls.png", appPath); // Path to controls.png

    SDL_Surface *controlsSurface = IMG_Load(controlsPath);
    if (controlsSurface) {
        SDL_Texture *controlsTexture = SDL_CreateTextureFromSurface(renderer, controlsSurface);
        SDL_FreeSurface(controlsSurface);

        int padding = 30;      // Padding for the sides
        int bottomLift = 50;   // Distance from the bottom of the screen

        int controlsWidth = SCREEN_WIDTH - 2 * padding;  // Full screen width minus padding
        int controlsHeight;

        // Maintain aspect ratio based on the adjusted width
        SDL_QueryTexture(controlsTexture, NULL, NULL, NULL, &controlsHeight);
        float aspectRatio = (float)controlsHeight / SCREEN_WIDTH;
        controlsHeight = (int)(controlsWidth * aspectRatio);

        // Position `controls.png` with padding and lift from the bottom
        SDL_Rect controlsRect = {padding, SCREEN_HEIGHT - controlsHeight - bottomLift, controlsWidth, controlsHeight};
        SDL_RenderCopy(renderer, controlsTexture, NULL, &controlsRect);

        SDL_DestroyTexture(controlsTexture);
        printf("Controls image loaded from: %s\n", controlsPath);
    } else {
        SDL_Log("Failed to load controls image from %s: %s", controlsPath, IMG_GetError());
    }
}


void renderGameName(SDL_Renderer *renderer, const char *name, int x, int y, int caseWidth) {
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, name, textColor);
    if (textSurface) {
        int textX = x + (caseWidth - textSurface->w) / 2;
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {textX, y, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    } else {
        SDL_Log("Failed to render text: %s", TTF_GetError());
    }
}


void renderGameIndexTopLeft(SDL_Renderer *renderer) {
    char indexText[50];
    snprintf(indexText, sizeof(indexText), "Game %d of %d", selected_game + 1, game_count);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, indexText, textColor);
    if (textSurface) {
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {10, 10, textSurface->w, textSurface->h}; // Top-left corner position
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    } else {
        SDL_Log("Failed to render game index text: %s", TTF_GetError());
    }
}
// Voeg de functie toe om het informatiescherm te renderen
void renderGameInfo(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Zwarte achtergrond
    SDL_RenderClear(renderer);

    Game selected = games[selected_game];

    // Render de banner achter alle andere elementen
    SDL_Surface *bannerSurface = IMG_Load(selected.banner_path);
    if (bannerSurface) {
        SDL_Texture *bannerTexture = SDL_CreateTextureFromSurface(renderer, bannerSurface);
        SDL_Rect bannerRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
        SDL_RenderCopy(renderer, bannerTexture, NULL, &bannerRect);
        SDL_FreeSurface(bannerSurface);
        SDL_DestroyTexture(bannerTexture);
    }

    // Render de grijze achtergrond aan de onderkant van het scherm
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Zwarte kleur voor onderste achtergrond
    SDL_Rect blackBackgroundRect = {0, SCREEN_HEIGHT / 2 + 150, SCREEN_WIDTH, SCREEN_HEIGHT / 2 - 150};
    SDL_RenderFillRect(renderer, &blackBackgroundRect);

    // Render de bovenste tekst "press button 1 or start to start"
    SDL_Color textColor = {255, 255, 255, 255}; // Witte tekstkleur
    SDL_Surface *topTextSurface = TTF_RenderText_Blended(font, "press button 1 or start to start    press button 2 or escape/return to return to game select", textColor);
    if (topTextSurface) {
        int textX = SCREEN_WIDTH / 2 - topTextSurface->w / 2;
        int textY = 20; // Bovenaan het scherm
        SDL_Texture *topTextTexture = SDL_CreateTextureFromSurface(renderer, topTextSurface);
        SDL_Rect topTextRect = {textX, textY, topTextSurface->w, topTextSurface->h};
        SDL_RenderCopy(renderer, topTextTexture, NULL, &topTextRect);
        SDL_FreeSurface(topTextSurface);
        SDL_DestroyTexture(topTextTexture);
    }

    // Render de cover art aan de linkerkant
// Render the cover art on the left side
    SDL_Surface *coverSurface = IMG_Load(selected.logo_path);
    if (coverSurface) {
        SDL_Texture *coverTexture = SDL_CreateTextureFromSurface(renderer, coverSurface);
        SDL_Rect coverRect = {SCREEN_WIDTH / 4 - 150, SCREEN_HEIGHT / 2 - 200, 300, 400};
        SDL_RenderCopy(renderer, coverTexture, NULL, &coverRect);
        SDL_FreeSurface(coverSurface);
        SDL_DestroyTexture(coverTexture);

        // Render the game title below the cover art
        SDL_Color textColor = {255, 255, 255, 255}; // White text color
        SDL_Surface *titleSurface = TTF_RenderText_Blended(font, selected.name, textColor);
        if (titleSurface) {
            SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
            SDL_Rect titleRect = {coverRect.x + (coverRect.w - titleSurface->w) / 2, coverRect.y + coverRect.h + 10, titleSurface->w, titleSurface->h};
            SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
            SDL_FreeSurface(titleSurface);
            SDL_DestroyTexture(titleTexture);
        }
    } else {
        SDL_Log("Failed to load cover art: %s", IMG_GetError());
    }


    // Render de titel en informatie uit info.txt in het midden met een doorzichtige achtergrond
    SDL_SetRenderDrawColor(renderer, 211, 211, 211, 180); // Lichte grijze doorzichtige achtergrond
    FILE *infoFile = fopen(selected.info_text, "r");
    char infoContent[1024] = "";
    if (infoFile) {
        fread(infoContent, sizeof(char), 1024, infoFile);
        for (int i = 0; i < 1024; ++i) {
            if (infoContent[i] == '	') {
                infoContent[i] = ' ';
            }
        }

        fclose(infoFile);
    }
    SDL_Surface *infoSurface = TTF_RenderText_Blended_Wrapped(font, infoContent, textColor, 500);
    if (infoSurface) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180); // Zachte zwarte doorzichtige achtergrond
        SDL_Rect infoBackgroundRect = {SCREEN_WIDTH / 2 - infoSurface->w / 2 - 10, SCREEN_HEIGHT / 2, infoSurface->w + 20, infoSurface->h + 20};
        SDL_RenderFillRect(renderer, &infoBackgroundRect);

        int textX = SCREEN_WIDTH / 2 - infoSurface->w / 2;
        int textY = SCREEN_HEIGHT / 2 + 10; // Midden van het scherm, iets onder de achtergrond
        SDL_Texture *infoTexture = SDL_CreateTextureFromSurface(renderer, infoSurface);
        SDL_Rect infoRect = {textX, textY, infoSurface->w, infoSurface->h};
        SDL_RenderCopy(renderer, infoTexture, NULL, &infoRect);
        SDL_FreeSurface(infoSurface);
        SDL_DestroyTexture(infoTexture);
    }

    // Render de credits uit credits.txt aan de rechterkant met een doorzichtige achtergrond
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180); // Zachte zwarte doorzichtige achtergrond
    FILE *creditsFile = fopen(selected.credits_text, "r");
    char creditsContent[1024] = "";
    if (creditsFile) {
        fread(creditsContent, sizeof(char), 1024, creditsFile);
        for (int i = 0; i < 1024; ++i) {
            if (creditsContent[i] == '	') {
                creditsContent[i] = ' ';
            }
        }

        fclose(creditsFile);
    }
    SDL_Surface *creditsSurface = TTF_RenderText_Blended_Wrapped(font, creditsContent, textColor, 500);
    if (creditsSurface) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180); // Zachte zwarte doorzichtige achtergrond
        SDL_Rect creditsBackgroundRect = {SCREEN_WIDTH / 2 + 250, SCREEN_HEIGHT / 2, creditsSurface->w + 20, creditsSurface->h + 20};
        SDL_RenderFillRect(renderer, &creditsBackgroundRect);

        int textX = SCREEN_WIDTH / 2 + 260;
        int textY = SCREEN_HEIGHT / 2 + 10; // Midden van het scherm, iets onder de achtergrond
        SDL_Texture *creditsTexture = SDL_CreateTextureFromSurface(renderer, creditsSurface);
        SDL_Rect creditsRect = {textX, textY, creditsSurface->w, creditsSurface->h};
        SDL_RenderCopy(renderer, creditsTexture, NULL, &creditsRect);
        SDL_FreeSurface(creditsSurface);
        SDL_DestroyTexture(creditsTexture);
    }

    SDL_RenderPresent(renderer);
}





void renderOverview(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderLogo(renderer);

    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    int caseWidth = 150;
    int caseHeight = 200;
    int spacing = 75;
    float scaleSelected = 1.5f;
    float linearProgress = anim_progress;
    float interpolatedOffset = linearProgress * (caseWidth + spacing) * direction;

    int startX = centerX - (caseWidth * scaleSelected) / 2 - interpolatedOffset;
    int selectedGamePosX = 0;
    int selectedGamePosY = 0;

    // Display game index in the top-left corner
    renderGameIndexTopLeft(renderer);

    for (int i = -game_count * 2; i <= game_count * 2; i++) {
        int index = (selected_game + i + game_count) % game_count;
        int posX = startX + i * (caseWidth + spacing);
        int yOffset = (i == 0 && anim_progress < 1.0f) ? 50 : 0;
        int constantOffset = (i != 0) ? 50 : yOffset;
        int posY = centerY - caseHeight / 2 + constantOffset;
        if (posX + caseWidth < 0 || posX > SCREEN_WIDTH) continue;

        float scale = (i == 0 && anim_progress == 1.0f) ? scaleSelected : 1.0f;
        int scaledWidth = caseWidth * scale;
        int scaledHeight = caseHeight * scale;
        int adjustedPosX = posX - (scaledWidth - caseWidth) / 2;

        SDL_Texture *texture = NULL;
        SDL_Surface *logo = IMG_Load(games[index].logo_path);

        if (logo) {
            texture = SDL_CreateTextureFromSurface(renderer, logo);
            SDL_SetTextureAlphaMod(texture, 255);
            SDL_FreeSurface(logo);
        } else {
            SDL_SetRenderDrawColor(renderer, 169, 169, 169, 255);
            SDL_Rect placeholderRect = {adjustedPosX, posY, scaledWidth, scaledHeight};
            SDL_RenderFillRect(renderer, &placeholderRect);
        }

        if (texture) {
            SDL_Rect dest_rect = {adjustedPosX, posY, scaledWidth, scaledHeight};
            SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
            SDL_DestroyTexture(texture);
        }

        if (i == 0) { // Highlight the selected game
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Gold border
            SDL_Rect borderRect = {adjustedPosX - 5, posY - 5, scaledWidth + 10, scaledHeight + 10};
            SDL_RenderDrawRect(renderer, &borderRect);

            // Store the position of the selected game
            selectedGamePosX = adjustedPosX + scaledWidth / 2;
            selectedGamePosY = posY;

            // Display game name above the selected game
            renderGameName(renderer, games[index].name, adjustedPosX, posY - 30, scaledWidth);
        }
    }

    renderControls(renderer);
    SDL_RenderPresent(renderer);
}



void updateAnimation() {
    Uint32 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    float speedFactor = 5.0f; // Pas deze factor aan voor snellere animati
    lastTime = currentTime;


    if (anim_progress < 1.0f) {
        anim_progress += anim_speed * deltaTime * speedFactor;
        if (anim_progress >= 1.0f) {
            anim_progress = 1.0f;
            selected_game = (selected_game + direction + game_count) % game_count;
            direction = 0;
        }
    }
}


int main(int argc, char* argv[]) {
    lastTime = SDL_GetTicks(); // Initialiseer lastTime aan het begin van main
    // Initial SDL and SDL_mixer setup
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL could not initialize: %s", SDL_GetError());
        return 1;
    }
    if (TTF_Init() == -1) {
        SDL_Log("TTF could not initialize: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    initAudio();

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("SDL_image could not initialize: %s", IMG_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    getAppPath();
    setupPaths();
    loadGames();

    SDL_Window* window = SDL_CreateWindow("Arcade Game Selector",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_ShowCursor(SDL_DISABLE);

    srand(time(NULL));
    Uint32 initial_delay = 10000; // Wacht 10 seconden voor de eerste track
    SDL_AddTimer(initial_delay, scheduleTrackPlayback, NULL);

    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE) {
                    running = 0;
                }

                if (current_screen == SCREEN_GAME_SELECT) {
                    if (e.key.keysym.sym == SDLK_RIGHT || e.key.keysym.sym == SDLK_d) {
                        if (direction == 0 && anim_progress >= 1.0f) {
                            direction = 1;
                            anim_progress = 0.0f;
                        }
                    } else if (e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_a) {
                        if (direction == 0 && anim_progress >= 1.0f) {
                            direction = -1;
                            anim_progress = 0.0f;
                        }
                    } else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_1 || e.key.keysym.sym == SDLK_z) {
                        current_screen = SCREEN_GAME_INFO;  // Ga naar het "Game Info" scherm
                    }
                } else if (current_screen == SCREEN_GAME_INFO) {
                    if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_3 || e.key.keysym.sym == SDLK_c) {
                        current_screen = SCREEN_GAME_SELECT;  // Ga terug naar het "Game Select" scherm
                    } else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_1 || e.key.keysym.sym == SDLK_z) {
                        Game selected = games[selected_game];
                        if (strlen(selected.exe_path) > 0) {
                            ShellExecute(NULL, "open", selected.exe_path, NULL, NULL, SW_SHOWNORMAL);
                        }
                    }
                }
            }
        }

        updateAnimation();

        // Wissel tussen schermen op basis van de huidige schermstatus
        if (current_screen == SCREEN_GAME_SELECT) {
            renderOverview(renderer);
        } else if (current_screen == SCREEN_GAME_INFO) {
            renderGameInfo(renderer);
        }

        SDL_Delay(0);
    }

    Mix_CloseAudio();
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
