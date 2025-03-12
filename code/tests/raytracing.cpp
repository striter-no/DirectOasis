#include <ConsoleAPI/HID/ansi_keyboard.hpp>
#include <ConsoleAPI/HID/mouse.hpp>
#include <ConsoleAPI/capi.hpp>

#include <ConsoleAPI/raytracing/shaders.hpp>

Shader shader;

void text(
    Console &console,
    std::wstring text,
    int x, int y,
    std::wstring color = L""
){
    for (int i = 0; i < text.size(); ++i) {
        console.pixel(x + i, y, Pixel(std::wstring{} + text[i], color));
    }
}

Pixel pixel(float x, float y, float aspectRatio){
    Ray ray;
    bool intersected = shader.proceed(x, y, aspectRatio, ray);

    if (intersected) {
        return Pixel(L" ", conv(colors::rgb_back(ray.color.r * 255, ray.color.g * 255, ray.color.b * 255)));
    }
    return Pixel(L" ", conv(colors::rgb_back(0, 0, 0)));
}

int main(){
    Console console( 
        Pixel(L".", conv(colors::rgb_back(255, 0, 255)))
    );
    auto &term = console.get_terminal();
    term.enableRawInput();
    
    console.hide_cursor();

    Mouse mouse(term);
    Keyboard keyboard(term);
    mouse.start();

    auto &cam = shader.getCamera();
    cam.rotation = glm::vec3(.0f, .0f, .0f);
    cam.position = glm::vec3(0.0f, 0.0f, 0.0f);

    auto &light = shader.light;
    light.direction = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));

    shader.addObject(
        Object(
            Material(glm::vec3(0.8f, 0.3f, 0.3f), 0.1f, 0.9f),
            std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, -5.0f), 1.0f)
        )
    );

    shader.addObject(
        Object(
            Material(glm::vec3(0.2f, 0.5f, 0.3f), 0.1f, 0.9f),
            std::make_shared<Plane>(
                glm::vec3(0, 0, -5), glm::vec3(0, 0, 1)
            )
        )
    );

    float dt = 5000; 
    float termAspect = 9/16.f;

    int tick = 0;
    while(!term.isCtrlCPressed()){
        keyboard.pollEvents();
        mouse.pollEvent();

        console.clear();
        auto [x, y] = mouse.getPosition();

        if (keyboard.isKeyPressed(L"w")){
            cam.moveForward(0.1);
        } else if (keyboard.isKeyPressed(L"s")){
            cam.moveForward(-0.1);
        }

        if (keyboard.isKeyPressed(L"a")){
            cam.moveRight(-0.1);
        } else if (keyboard.isKeyPressed(L"d")){
            cam.moveRight(0.1);
        }

        if (keyboard.isKeyPressed(L"e")){
            cam.moveUp(0.1);
        } else if (keyboard.isKeyPressed(L"q")){
            cam.moveUp(-0.1);
        }

        // cam.rotateFromMouse(1, 1);

        for (int y = 0; y < console.height; ++y) {
            for (int x = 0; x < console.width; ++x) {
                console.pixel(x, y, pixel(
                    (float)x / console.width,
                    (float)y / console.height,
                    console.width / console.height * termAspect
                ));
            }
        }

        text(console, L"Camera Position: " + std::to_wstring(cam.position.x) + L' ' + std::to_wstring(cam.position.y) + L' ' + std::to_wstring(cam.position.z), 1, 1, conv(colors::Fore.white));
        text(console, L"Camera Rotation: " + std::to_wstring(cam.getViewVector().x) + L' ' + std::to_wstring(cam.getViewVector().y) + L' ' + std::to_wstring(cam.getViewVector().z), 1, 3, conv(colors::Fore.white));

        console.draw();
        usleep(dt);
        tick++;
    }

    term.disableMouse();
    term.showCursor();
    term.restoreInput();
}