#include <ConsoleAPI/raytracing/app.hpp>

int main(){
    DirectOasis app(GRAPHICS_TYPE::RAY_TRACING);

    app.setup(false, false);

    const auto &tick = app.getTicks();
    auto &console = app.getConsole();
    auto [sw, sh] = app.getSimbolsSize();
    while (!app.needStop()){
        app.update(
            [&](){
                ;
            },
            [&](){
                for (int y = 0; y < sh; y++){ 
                    for (int x = 0; x < sw; x++){
                        float 
                            nx = (float)x / sw, 
                            ny = (float)y / sh
                        ;

                        if(!console.pixel(x, y, Pixel(" ", 
                            colors::rgb_back(
                                nx * 255, 
                                ny * 255, 
                                (sin(tick/10*M_PI)+1) * 0.5 * 255
                            ), true
                        ), false)) throw std::runtime_error("Draw error on " + std::to_string(x) + ", " + std::to_string(y));
                    }
                }

                app.text(0, 4, "Pixel: " + std::to_string((4 / sw) * 255) + " " + std::to_string((4 / sh) * 255) + " " + std::to_string((sin(tick/10*M_PI)+1)* 0.5 * 255) , colors::Fore.white);

                app.text(0, 0, "FPS: " + std::to_string(app.getFPS(app.getConsoleElapsed())) + " (" + std::to_string(app.getConsoleElapsed()) + " milliseconds)", colors::Fore.white);
                app.text(0, 1, "Resolution: " + std::to_string(sw) + " " + std::to_string(sh), colors::Fore.white);
                app.text(0, 2, "Ticks: " + std::to_string(tick), colors::Fore.white);
            }, 0.f, true, false
        );
    }

    return 0;
}