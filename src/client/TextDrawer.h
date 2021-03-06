//
// Created by javif on 01/11/2019.
//

#ifndef MICROMACHINES_TEXTDRAWER_H
#define MICROMACHINES_TEXTDRAWER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <sstream>
#include <iomanip>

class TextDrawer {
    SDL_Renderer* renderer;
    TTF_Font* font;
public:
    explicit TextDrawer(SDL_Renderer* renderer) : renderer(renderer){
        TTF_Init();
        font = TTF_OpenFont("assets/fonts/small_pixel-7.ttf", 48);
    }

    TextDrawer& operator=(TextDrawer&& other) noexcept{
        this->font = other.font;
        this->renderer = other.renderer;
        other.font = nullptr;
        return *this;
    }

    void drawText(const std::string& orig_text, int x, int y, int h, int shadow_dist, int padding){
        std::stringstream ss;
        ss << std::setw(padding) << orig_text;
        std::string text = ss.str();
        int w = h*text.size()*0.8;
        SDL_Surface* surface_b = TTF_RenderUTF8_Solid(font, text.c_str(), SDL_Color{0,0,0});
        SDL_Surface* surface_w = TTF_RenderUTF8_Solid(font, text.c_str(), SDL_Color{0xff,0xff,0xff});
        SDL_Texture* texture_b = SDL_CreateTextureFromSurface(renderer, surface_b);
        SDL_Texture* texture_w = SDL_CreateTextureFromSurface(renderer, surface_w);
        SDL_Rect dst_w = {x, y, w, h};
        SDL_Rect dst_b = {x+shadow_dist, y+shadow_dist, w, h};
        SDL_RenderCopy(renderer, texture_b, nullptr, &dst_b);
        SDL_RenderCopy(renderer, texture_w, nullptr, &dst_w);
        SDL_DestroyTexture(texture_w);
        SDL_DestroyTexture(texture_b);
        SDL_FreeSurface(surface_w);
        SDL_FreeSurface(surface_b);
    }

    ~TextDrawer(){
        if (font){
            TTF_CloseFont(font);
            TTF_Quit();
        }
    }
};

#endif //MICROMACHINES_TEXTDRAWER_H
