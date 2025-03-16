#pragma once

#include <utils/vector.hpp>
#include <utils/string.hpp>

#include "colors.hpp"
#include "terminal.hpp"

std::wstring conv(std::string str){
    std::wstring result;
    result.append(str.begin(), str.end());
    return result;
}

class Pixel {
    public:
        bool is_wide = false;
        std::wstring data = L" ";
        std::wstring color = L"";
        
        std::string nrw_data = " ";
        std::string nrw_color = "";

        std::wstring compiled;
        std::string  nrw_compiled;

        Pixel(std::wstring data, std::wstring color = L"", bool pre_compiled = true):
            data(data), color(color), is_wide(true) {
            if (pre_compiled) {
                compiled = color + data;
            }
        }
        Pixel(std::string data, std::string color = "", bool pre_compiled = true):
            nrw_data(data), nrw_color(color), is_wide(false) {
            if (pre_compiled) {
                nrw_compiled = nrw_color + nrw_data;
            }
        }
        Pixel(){}

        std::wstring compile(bool check_pre_compiled = true){
            // throw std::runtime_error("Console not initialized");
            // if (check_pre_compiled && compiled != L"") 
            return compiled;
            // return color + data; 
        }

        std::string &narrow_compile(bool check_pre_compiled = true){ 
            return nrw_compiled;
            // if (check_pre_compiled && nrw_compiled != "") return nrw_color + nrw_data; 
        }

        bool operator==(const Pixel &other) const {
            return data == other.data && color == other.color;
        }
};

class Console{
        Terminal terminal;
        
        std::vector<std::vector<Pixel>> data;
        Pixel bg_pixel;
    
        void ini_fill(int width, int height){
            std::vector<std::vector<Pixel>> cols(height);
            for (int y = 0; y < height; y++){
                std::vector<Pixel> row(width);
                for (int x = 0; x < width; x++){
                    row[x] = bg_pixel;
                }
                cols[y] = row;
            }
            data = cols;
        }

    public:
        int width = 1, height = 1;

        Console(
            int width,
            int height,
            Pixel bg_pixel,
            bool vt = true
        ): width(width), height(height), bg_pixel(bg_pixel) {
            terminal.startANSI();
            setup();
            ini_fill(width, height);

            if (vt)
                terminal.enableVT();
        }

        Console(
            Pixel bg_pixel,
            bool vt = true
        ): width(terminal.getSize().first - 1), height(terminal.getSize().second - 1), bg_pixel(bg_pixel) {
            terminal.startANSI();
            setup();
            ini_fill(width, height);

            if (vt)
                terminal.enableVT();
        }

        void setup(){
            #ifdef _WIN32
            system("chcp 65001 > nul");
            #endif

            std::ios_base::sync_with_stdio(false);
            std::locale::global(std::locale("en_US. UTF-8"));
            std::wcout.imbue(std::locale());
        }

        bool is_valid(int x, int y){
            return (x >= 0 && x < width && y >= 0 && y < height);
        }

        Pixel &get_pixel(int x, int y){
            return data[y][x];
        }

        void clear(){
            ini_fill(width, height);
            terminal.clear();
        }

        void draw(){
            std::wstring buffer;
            for (int y = 0; y < height; y++){
                for (int x = 0; x < width; x++){
                    buffer += data[y][x].compile();
                }
                buffer += L"\n";
            }
            terminal.draw(buffer + conv(colors::Fore.reset));
        }

        void narrow_draw(bool crossing = false){

            std::string buffer;
            if (!crossing)
                for (int y = 0; y < height; y++){
                    for (int x = 0; x < width; x++){
                        buffer += data[y][x].narrow_compile();
                    }
                    buffer += "\n";
                }
            else
                for (int y = 0; y < height; y++){
                    for (int x = 0; x < width; x++){
                        if (!data[y][x].is_wide) buffer += data[y][x].narrow_compile();
                        else buffer += wideToUTF8(data[y][x].compile());
                    }
                    buffer += "\n";
                }
            terminal.draw(buffer + colors::Fore.reset);
        }

        void hide_cursor(){
            terminal.hideCursor();
        }

        void show_cursor(){
            terminal.showCursor();
        }

        bool pixel(int x, int y, Pixel pixel, bool safe = true){
            // throw std::runtime_error("X: " + std::to_string(x) + ", Y: " + std::to_string(y) + ", Pixel: " + pixel.narrow_compile().substr(1));
            if (!is_valid(x, y) && safe) return false;
            // if (!pixel.color.empty() || ) pixel.data += conv(colors::Fore.reset);
            data[y][x] = pixel;
            return true;
        }

        bool opti_pixel(int x, int y, Pixel _pixel, bool safe = true){
            if (!is_valid(x, y) && safe) return false;
            if (data[y][x] == _pixel) return false;
            return pixel(x, y, _pixel);
        }

        Terminal &get_terminal(){
            return terminal;
        }

        Console(){}
        ~Console(){}
};