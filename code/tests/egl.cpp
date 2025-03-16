#include <ConsoleAPI/raytracing/app.hpp>
#include <ConsoleAPI/gpu_graphics/renderer.hpp>

int main(){
    DirectOasis app(GRAPHICS_TYPE::RAY_TRACING);
    auto [sw, sh] = app.getSimbolsSize();
    app.setup(false, false);
    
    DirectGL       gl(sw, sh);
    DirectGLShader shader("./code/glsl/ConsoleAPI/general");
    gl.setup();
    
    Renderer       renderer(shader, sw, sh);

    const auto &tick = app.getTicks();
    auto &console = app.getConsole();
    while (!app.needStop()){
        app.update(
            [&](){
                ;
            },
            [&](){
                Image image = renderer.draw([&](DirectGLShader& shader){});
                for (int y = 0; y < sh; y++){ 
                    for (int x = 0; x < sw; x++){
                        if(!console.pixel(x, y, Pixel(" ", 
                            colors::rgb_back(
                                image.pixels[y][x].r * 255.f, 
                                image.pixels[y][x].g * 255.f, 
                                image.pixels[y][x].b * 255.f
                            ), true
                        ), false)) throw std::runtime_error("Draw error on " + std::to_string(x) + ", " + std::to_string(y));
                    }
                }

                auto &samplePixel = console.get_pixel(0, 0);
                app.text(0, 4, "Pixel: " + samplePixel.nrw_color.substr(1), colors::Fore.white);

                app.text(0, 0, 
                    "FPS: " + std::to_string(app.getFPS(app.getConsoleElapsed())) + " (" + std::to_string(app.getConsoleElapsed()) + " milliseconds)", 
                    colors::Fore.white
                );

                app.text(0, 1, "Resolution: " + std::to_string(sw) + " " + std::to_string(sh), colors::Fore.white);
                app.text(0, 2, "Ticks: " + std::to_string(tick), colors::Fore.white);
            }, 0.f, true, false
        );
    }

    gl.finish();
    return 0;
}