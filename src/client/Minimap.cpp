//
// Created by javier on 3/11/19.
//

#include "Minimap.h"

Minimap::Minimap() : my_car_id(-1), size_x(1), size_y(1) {
    surface = SDL_CreateRGBSurfaceWithFormat(0, 300, 300, 32,
            SDL_PIXELFORMAT_RGBA32);
    white_s = SDL_CreateRGBSurface(0, 7, 7, 32, 0, 0, 0, 0);
    SDL_FillRect(white_s, nullptr,
                 SDL_MapRGB(white_s->format, 255, 255, 255));
    black_s = SDL_CreateRGBSurface(0, 7, 7, 32, 0, 0, 0, 0);
    SDL_FillRect(black_s, nullptr,
                 SDL_MapRGB(white_s->format, 0, 0, 0));
}
void Minimap::setMyID(int32_t id){
    my_car_id = id;
}
void Minimap::addTile(TileInfo& info){
    if ((info.type >= 3 && info.type <= 27) || (info.type >= 29 && info.type <= 32) || (info.type >= 34 && info.type <= 55 && info.type != 36)){
        SDL_Rect dst_w{300*info.posx/size_x, 300*info.posy/size_y, 4, 4};
        SDL_Rect dst_b{300*info.posx/size_x+10, 300*info.posy/size_y+10, 4, 4};
        SDL_BlitSurface(black_s, nullptr, surface, &dst_b);
        SDL_BlitSurface(white_s, nullptr, surface, &dst_w);
    }
}
void Minimap::updateCar(int32_t id, int32_t x, int32_t y){
    auto& car = cars.at(id);
    car.x = MINIMAP_WIDTH*x/size_x;
    car.y = MINIMAP_HEIGHT*y/size_y + 0.8f;
}
void Minimap::addCar(int32_t id, int32_t x, int32_t y){
    cars.emplace(id, MiniCar{MINIMAP_WIDTH*x/size_x, MINIMAP_HEIGHT*y/size_y+(1-MINIMAP_WIDTH)});
}
void Minimap::removeCar(int32_t id){
    cars.erase(id);
}
void Minimap::draw(Camera& camera){
    camera.drawSurface(surface, 0.05, 0.75, MINIMAP_WIDTH, MINIMAP_HEIGHT);
    int32_t tex;
    for (auto& car : cars) {
        if (car.first == my_car_id)
            tex = RED_DOT;
        else
            tex = GREY_DOT;
        camera.drawScreenTexture(tex, car.second.x + 0.05,
                                 car.second.y - 0.05, 0.5);
    }
}
void Minimap::setSize(int32_t w, int32_t h){
    size_x = w*TILE_TERRAIN_SIZE*METER_TO_PIXEL;
    size_y = h*TILE_TERRAIN_SIZE*METER_TO_PIXEL;
}
Minimap::~Minimap(){
    SDL_FreeSurface(white_s);
    SDL_FreeSurface(surface);
    SDL_FreeSurface(black_s);
}
