//
// Created by javier on 10/11/19.
//

#include <SDL_messagebox.h>
#include "ScreenRecorder.h"

ScreenRecorder::ScreenRecorder() : width(0), height(0), recording(false), recording_texture(nullptr), queue(60*60), writer(queue){
    writer.start();
}

void ScreenRecorder::startRecording(SDL_Renderer* renderer, int w, int h) {
    this->width = w;
    this->height = h;
    recording_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, width, height);
    buffer.clear();
    buffer.resize(w*h*3);
    writer.setup(w, h);
    //queue.reset(new ProtectedQueue<std::vector<uint8_t>>(60*60));
    //writer.reset(new ThreadWriter(queue, w, h));
    //writer->start();
    this->recording = true;
}

void ScreenRecorder::recordFrame(SDL_Renderer *renderer) {
    if (recording) {
        buffer.resize(width*height*3);
        int res = SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGB24, buffer.data(), width*3);
        if (res){
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "RendererReadPixels error", SDL_GetError(), nullptr);
        }
        if (!queue.full())
            queue.push(std::move(buffer));
    }
}

void ScreenRecorder::stopRecording() {
    this->recording = false;
    writer.saveVideo();
    //queue->close();
    //writer->shutdown();
    //writer->join();
    //writer.reset();
    //queue.reset();
    SDL_DestroyTexture(recording_texture);
}

bool ScreenRecorder::isRecording() {
    return recording;
}

ScreenRecorder::~ScreenRecorder() {
    if (recording) {
        stopRecording();
    }
    if (recording_texture){
        SDL_DestroyTexture(recording_texture);
    }
    queue.close();
    writer.join();
}

SDL_Texture *ScreenRecorder::getRecordingTexture() {
    return recording_texture;
}

