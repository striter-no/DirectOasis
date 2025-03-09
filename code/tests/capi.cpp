#include <ConsoleAPI/capi.hpp>
#include <unistd.h>

int main(){
    Console console(
        20, 10, Pixel(L".")
    );

    console.hide_cursor();
    while(true){
        console.clear();
        console.draw();
        
        usleep(800);
    }
}