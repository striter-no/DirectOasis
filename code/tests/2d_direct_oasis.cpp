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

                        console.pixel(x, y, Pixel(L" ", conv(
                            colors::rgb_back(
                                nx * 255, 
                                ny * 255, 
                                (sin(tick/10*M_PI)+1) * 0.5 * 255
                            )
                        )));
                    }
                }

                app.text(0, 0, L"FPS: " + std::to_wstring(app.getFPS()), conv(colors::Fore.white));
                app.text(0, 1, L"Resolution: " + std::to_wstring(sw) + L" " + std::to_wstring(sh), conv(colors::Fore.white));
                app.text(0, 2, L"Ticks: " + std::to_wstring(tick), conv(colors::Fore.white));
            }, 0.f
        );
    }

    return 0;
}