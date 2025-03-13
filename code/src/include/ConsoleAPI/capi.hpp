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

struct Pixel {
    std::wstring data = L" ";
    std::wstring color = L"";

    Pixel(std::wstring data, std::wstring color = L""):
        data(data), color(color) {}
    Pixel(){}

    std::wstring compile(){
        return color + data;
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

        void hide_cursor(){
            terminal.hideCursor();
        }

        void show_cursor(){
            terminal.showCursor();
        }

        bool pixel(int x, int y, Pixel pixel){
            if (!is_valid(x, y)) return false;
            if (!pixel.color.empty()) pixel.data += conv(colors::Fore.reset);
            data[y][x] = pixel;
            return true;
        }

        Terminal &get_terminal(){
            return terminal;
        }

        Console(){}
        ~Console(){}
};