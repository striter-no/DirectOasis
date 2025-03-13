#include <ConsoleAPI/HID/ansi_keyboard.hpp>
#include <ConsoleAPI/HID/mouse.hpp>
#include <ConsoleAPI/capi.hpp>
#include <ConsoleAPI/raytracing/chrono_utils.hpp>

#include <ConsoleAPI/raytracing/shaders.hpp>

Shader shader = Shader(GRAPHICS_TYPE::RAY_TRACING);

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

glm::mat2 rot(float a) {
    float s = glm::sin(a);
    float c = glm::cos(a);
    return glm::mat2(c, -s, s, c);
}

Pixel pixel(float x, float y, float aspectRatio){
    Ray ray;
    shader.proceed(x, y, aspectRatio, ray);
    return Pixel(L" ", conv(colors::rgb_back(ray.color.r * 255, ray.color.g * 255, ray.color.b * 255)));
}

int main(){
    Console console( 
        Pixel(L".", conv(colors::rgb_back(255, 0, 255)))
    );
    auto &term = console.get_terminal();
    term.enableRawInput();
    
    console.hide_cursor();

    Mouse mouse(term);
    WindowMouse winmouse;
    Keyboard keyboard(term);
    mouse.start();

    auto &light = shader.light;
    light.direction = glm::normalize(glm::vec3(-0.642437, 0.600000, 0.476734 ));

    Material common;
    common.emissive = 0.f;
    common.transparency = 0.f;
    common.metallic = 0.2f;
    common.roughness = 0.1f;
    common.color = glm::vec3(0.7f, 0.7f, 0.7f);

    Material metallic = common;
    metallic.color = glm::vec3(0.8f, 0.1f, 0.8f);
    metallic.metallic = 8.f;

    Material transp = common;
    transp.color = glm::vec3(0.2f, 0.8f, 0.8f);
    transp.transparency = 0.5f;

    shader.addObject(
        Object(
            std::make_shared<Sphere>(
                glm::vec3(-5.0f, 0.0f, 0.0f), 1.0f, 
                transp
            )
        )
    );

    shader.addObject(
        Object(
            std::make_shared<Sphere>(
                glm::vec3(-5.0f, 0.0f, 5.0f), 2.0f, 
                metallic
            )
        )
    );

    shader.addObject(
        Object(
            std::make_shared<Box>(
                glm::vec3{0.f, 2.f, 2.f}, glm::vec3{1.f, 2.f, 1.f}, 
                common
            )
        )
    );

    shader.addObject(
        Object(
            std::make_shared<Box>(
                glm::vec3{2.f, 2.f, 0.f}, glm::vec3{1.f}, 
                transp
            )
        )
    );

    shader.addObject(
        Object(
            std::make_shared<Plane>(
                glm::vec3{0.f, 0.f, 1.f},
                common
            )
        )
    );

    float dt = 5000; 
    float termAspect = 9/16.f;

    int tick = 0;
    
    // if(time >= 1000){
    //     FPS = frames;
    //     glfwSetWindowTitle(window, (name + " | FPS: " + std::to_string(frames)).c_str());
    //     time = 0; frames = 0;
    // }

    // begin = extra::getChornoTimeNow();

    // winmouse.lockCursor();
    winmouse.hideMouse();
    bool in_game = true;

    int FPS = 0;
    int frames = 0;
    float time = 0;


    auto [w, h] = winmouse.getSize();
    float mx = 0, my = 0;
    glm::vec3 campos = {0, 0, 0};
    auto begin = extra::getChornoTimeNow();
    while(!term.isCtrlCPressed()){
        console.clear();

        if(time >= 1000){
            FPS = frames;
            time = 0; frames = 0;
        }

        begin = extra::getChornoTimeNow();
        for (int y = 0; y < console.height; ++y) {
            for (int x = 0; x < console.width; ++x) {
                console.pixel(x, y, pixel(
                    (float)x / console.width,
                    (float)y / console.height,
                    console.width / console.height * termAspect
                ));
            }
        }
        frames++;
        time += extra::getChronoElapsed(begin);

        winmouse.pollEvents();
        keyboard.pollEvents();

        if (in_game){
            auto [rx, ry] = winmouse.getPosition();
            winmouse.moveMouse(w / 2, h / 2);
            mx += rx - w / 2;
            my -= ry - h / 2;
        }

        float lmx = ((float)mx / w - 0.5f) * 0.5f, 
              lmy = ((float)my / h - 0.5f) * 0.5f;

        shader.setUniform("ux_mouse", rot( lmx));
        shader.setUniform("uy_mouse", rot(-lmy));
        
        if (keyboard.isKeyPressed(L"`")){
            in_game = false;
            winmouse.showMouse();
        }
        if (keyboard.isKeyPressed(L"1")){
            in_game = true;
            winmouse.hideMouse();
        }


        glm::vec3 dir = {0, 0, 0}, dir_t = {0, 0, 0};
        if (keyboard.isKeyPressed(L"w")){
            dir = glm::vec3(1, 0, 0);
        } else if (keyboard.isKeyPressed(L"s")){
            dir = glm::vec3(-1, 0, 0);
        }

        if (keyboard.isKeyPressed(L"a")){
            dir += glm::vec3(0, -1.f, 0);
        } else if (keyboard.isKeyPressed(L"d")){
            dir += glm::vec3(0, 1.f, 0);
        }


        dir_t.z = dir.z * cos(-lmy) - dir.x * sin(-lmy);
        dir_t.x = dir.z * sin(-lmy) + dir.x * cos(-lmy);
        dir_t.y = dir.y;
        dir.x = dir_t.x * cos(lmx) - dir_t.y * sin(lmx);
        dir.y = dir_t.x * sin(lmx) + dir_t.y * cos(lmx);
        dir.z = dir_t.z;

        if (keyboard.isKeyPressed(L"e")){
            campos.z += .1f;
        } else if (keyboard.isKeyPressed(L"q")){
            campos.z -= .1f;
        }

        campos += dir * .1f;

        shader.setUniform("cam_pos", campos);
        light.direction = glm::normalize(glm::vec3(cos(tick*0.01), -sin(tick*0.01), 0.75f));
        text(console, 
            L"Light Direction: " + 
                std::to_wstring(light.direction.x) + L' ' + 
                std::to_wstring(light.direction.y) + L' ' +
                std::to_wstring(light.direction.z), 1, 1, conv(colors::Fore.white));
        
        text(console, 
            L"FPS: " + 
                std::to_wstring(FPS), 1, 2, conv(colors::Fore.white));

        console.draw();
        usleep(dt);
        tick++;
    }

    mouse.stop();
    winmouse.showMouse();
    // winmouse.unlockCursor();
    term.disableMouse();
    term.showCursor();
    term.restoreInput();
}

/*


if (winmouse.isButtonPressed(MouseButton::LEFT)) {
    text(console, L"Camera Position: " + std::to_wstring(cam.position.x) + L' ' + std::to_wstring(cam.position.y) + L' ' + std::to_wstring(cam.position.z), 1, 1, conv(colors::Fore.white));
    text(console, L"Camera Rotation: " + std::to_wstring(cam.getForward().x) + L' ' + std::to_wstring(cam.getForward().y) + L' ' + std::to_wstring(cam.getForward().z), 1, 3, conv(colors::Fore.white));
} else {
    text(console, L"Mouse X Y: " + std::to_wstring(rx) + L' ' + std::to_wstring(ry), 1, 1, conv(colors::Fore.white));
}
*/