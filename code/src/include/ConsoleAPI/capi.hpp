#pragma once

#include <MathLib/VectorFuncs.hpp>
#include <utils/vector.hpp>
#include <utils/string.hpp>

#include <locale>

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
        int width = 1, height = 1;

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

        Console(
            int width,
            int height,
            Pixel bg_pixel
        ): width(width), height(height), bg_pixel(bg_pixel) {}

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
            std::wcout << "\033[H\033[2J";
        }

        void draw(){
            std::wstring buffer;
            for (int y = 0; y < height; y++){
                for (int x = 0; x < width; x++){
                    buffer += data[y][x].compile();
                }
                buffer += L"\n";
            }
            std::wcout << buffer << std::endl;
        }

        void hide_cursor(){
            std::wcout << L"\033[?25l";
        }

        void show_cursor(){
            std::wcout << L"\033[?25h";
        }

        bool pixel(int x, int y, Pixel pixel){
            if (!is_valid(x, y)) return false;
            data[y][x] = pixel;
            return true;
        }

        Console(){}
        ~Console(){}
};