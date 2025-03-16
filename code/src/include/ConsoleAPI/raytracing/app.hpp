#pragma once

#include <ConsoleAPI/HID/ansi_keyboard.hpp>
#include <ConsoleAPI/HID/mouse.hpp>
#include <ConsoleAPI/capi.hpp>
#include <ConsoleAPI/raytracing/chrono_utils.hpp>
#include <ConsoleAPI/raytracing/shaders.hpp>

#include <mutex>
#include <thread>

class DirectBuffer {
        float extraAspect = 1.f;
    public:
        std::vector<std::vector<Ray>> rays;
        std::vector<std::vector<glm::vec3>> colors;

        void draw(Console &console, bool by_rays = true, int start_x = 0, int start_y = 0){
            if (!by_rays){

                for (int y = start_y; y < colors.size(); ++y) {
                    for (int x = start_x; x < colors[0].size(); ++x) {
                        console.pixel(x, y, Pixel(
                            L" ", conv(colors::rgb_back(colors[y][x].r * 255, colors[y][x].g * 255, colors[y][x].b * 255) )
                        ));
                    }
                }
            } else if (by_rays){
                // throw std::runtime_error("Rays is not empty: " + std::to_string(rays.size()) + "x" + std::to_string(rays[0].size()));
                for (int y = start_y; y < rays.size(); ++y) {
                    for (int x = start_x; x < rays[0].size(); ++x) {
                        console.pixel(x, y, Pixel(
                            L" ", conv(colors::rgb_back(
                                rays[y][x].color.r * 255.f, 
                                rays[y][x].color.g * 255.f, 
                                rays[y][x].color.b * 255.f
                            ))
                        ));
                    }
                }
            } else {
                throw std::runtime_error("No data to draw");
            }
        }

        void shade(Shader &shader){
            int width = rays[0].size();
            int height = rays.size();
            // throw std::runtime_error("Rays size: " + std::to_string(rays.size()) + "x" + std::to_string(rays[0].size()));
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    shader.proceed((float)x / width, (float)y / height, (float)width / height * extraAspect, rays[y][x]);
                }
            }
        }

        void multithread_shade(Shader &shader, int threads_count) {
            std::vector<std::thread> threads;
            threads.reserve(threads_count); // Резервируем место
            int width = rays[0].size();
            int height = rays.size();

            for (int i = 0; i < threads_count; ++i) {
                threads.emplace_back([&, i]() { // Захватываем i по значению
                    int start_x = i * (width / threads_count);
                    int end_x = (i == threads_count - 1) ? width : (i + 1) * (width / threads_count);
                    for (int y = 0; y < height; ++y) {
                        for (int x = start_x; x < end_x; ++x) {
                            shader.proceed((float)x / width, (float)y / height, 
                                        (float)width / height * extraAspect, rays[y][x]);
                        }
                    }
                });
            }

            for (auto& t : threads) {
                if (t.joinable()) {
                    t.join();
                }
            }
        }

        DirectBuffer(int width, int height, float extraAspect = 1.f):
            rays(height, std::vector<Ray>(width)),
            colors(height, std::vector<glm::vec3>(width)),
            extraAspect(extraAspect)
        {}

        DirectBuffer(){}
        ~DirectBuffer(){}
};

class DirectOasis {
        WindowMouse win_mouse;
        Mouse       ansi_mouse;
        Keyboard    ansi_kboard;

        Console console;
        Shader  main_shader;
        
        int fps = 0, frames = 0, ticks = 0;
        
        float all_elapsed     = 0.f;
        float user_elapsed    = 0.f;
        float console_elapsed = 0.f;
        float system_elapsed  = 0.f;

    public:

        bool needStop(){
            return console.get_terminal().isCtrlCPressed();
        }

        void object(Object obj){
            main_shader.addObject(obj);
        }

        void text(int x, int y, std::wstring text, std::wstring color = L""){
            for (int i = 0; i < text.size(); ++i) {
                console.pixel(x + i, y, Pixel(std::wstring{} + text[i], color));
            }
        }

        void text(int x, int y, std::string text, std::string color = ""){
            for (int i = 0; i < text.size(); ++i) {
                console.pixel(x + i, y, Pixel(std::string{} + text[i], color));
            }
        }

        void setup(bool lockCursor = true, bool hideCursor = true){
            auto &term = console.get_terminal();
            term.enableRawInput();
            console.hide_cursor();

            if (lockCursor) win_mouse.lockCursor();
            if (hideCursor) win_mouse.hideMouse();

            ansi_mouse  = {&term};
            ansi_kboard = {&term};

            ansi_mouse.start();
            ansi_kboard.start();
        }

        void finish(){
            auto &term = console.get_terminal();
            win_mouse.unlockCursor();
            win_mouse.showMouse();

            ansi_kboard.stop();
            ansi_mouse.stop();

            term.disableMouse();
            term.showCursor();
            term.restoreInput();
        }

        void update(
            std::function<void()> user_update,
            std::function<void()> user_draw,
            float frame_sleep = 5000.f,
            bool narrow_draw = false,
            bool crossing_draw = false
        ){
            auto begin = extra::getChronoTimeNow();

            win_mouse.pollEvents();
            ansi_kboard.pollEvents();
            // ansi_mouse.pollEvents();
            system_elapsed = extra::getChronoElapsed(begin);
            auto user_begin = extra::getChronoTimeNow();
            user_update();

            console.clear();
            user_draw();
            user_elapsed = extra::getChronoElapsed(user_begin);
            auto console_begin = extra::getChronoTimeNow();

            if (narrow_draw) console.narrow_draw(crossing_draw);
            else console.draw();

            console_elapsed = extra::getChronoElapsed(console_begin);

            usleep(std::max(0.f, frame_sleep - extra::getChronoElapsed<std::chrono::microseconds>(begin)));
            frames++;
            ticks++;
            all_elapsed = extra::getChronoElapsed(begin);
        }

        DirectionalLight &getLight(){
            return main_shader.light;
        }

        const int &getTicks(){
            return ticks;
        }

        std::pair<int, int> getSimbolsSize(){
            return {console.width, console.height};
        }

        std::pair<int, int> getWindowSize(){
            return win_mouse.getSize();
        }

        Shader &getMainShader(){
            return main_shader;
        }

        Console &getConsole(){
            return console;
        }

        Terminal &getTerminal(){
            return console.get_terminal();
        }

        WindowMouse &getWinMouse(){
            return win_mouse;
        }

        Keyboard &getANSIKeyboard(){
            return ansi_kboard;
        }

        const int &getFrames(){
            return frames;
        }

        const float &getAllElapsed(){
            return all_elapsed;
        }

        const float &getUserElapsed(){
            return user_elapsed;
        }

        const float &getConsoleElapsed(){
            return console_elapsed;
        }

        const float &getSystemElapsed(){
            return system_elapsed;
        }

        int getFPS(float dt){
            if (dt == 0.f) return -1;

            return round(1000.f / dt);
        }

        DirectOasis(
            GRAPHICS_TYPE graphics_type
        ): main_shader(graphics_type), console(Pixel(L" ", conv(colors::rgb_back(255, 0, 255)))) {}

        DirectOasis(){}
        ~DirectOasis(){}
};