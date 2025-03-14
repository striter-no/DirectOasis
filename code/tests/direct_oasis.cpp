#include <ConsoleAPI/raytracing/app.hpp>

glm::mat2 rot(float a) {
    float s = glm::sin(a);
    float c = glm::cos(a);
    return glm::mat2(c, -s, s, c);
}

glm::vec3 red = glm::vec3(1.f, 0.3f, 0.2f);
glm::vec3 blue = glm::vec3(0.2f, 0.2f, 1.f);
glm::vec3 green = glm::vec3(0.3f, 1.f, 0.2f);
glm::vec3 yellow = glm::vec3(1.f, 1.f, 0.2f);
glm::vec3 purple = glm::vec3(1.f, 0.2f, 1.f);
glm::vec3 cyan = glm::vec3(0.2f, 1.f, 1.f);
glm::vec3 white = glm::vec3(1.f, 1.f, 1.f);
Material common = Material(0, 0, 0, 0), metallic, transp, emissive;

int main(){
    metallic = common; metallic.metallic = 8.f;
    transp   = common; transp.transparency = 0.5f;
    emissive = common; emissive.emissive = 1.f;

    float mx = 0, my = 0;
    bool in_game = true;
    glm::vec3 campos = {0, 0, 0};

    DirectOasis app(GRAPHICS_TYPE::RAY_TRACING);

    app.getMainShader().addObject({
        std::make_shared<Sphere>(
            glm::vec3(-5.0f, 0.0f, 0.0f), 1.0f, 
            transp, red
    )});

    app.getMainShader().addObject({
        std::make_shared<Plane>(
            glm::vec3{0.f, 0.f, 1.f},
            common, white
        )
    });

    app.setup();
    while (!app.needStop()){
        app.update(
            [&](){
                auto &shader   = app.getMainShader();
                auto &mouse    = app.getWinMouse();
                auto &keyboard = app.getANSIKeyboard();
                auto [w, h]    = app.getWindowSize();

                auto [rx, ry] = mouse.getPosition();
                mouse.moveMouse(w / 2, h / 2);
                mx += rx - w / 2;
                my -= ry - h / 2;
                
                float lmx = ((float)mx / w - 0.5f) * 0.5f, 
                      lmy = ((float)my / h - 0.5f) * 0.5f;
                
                shader.setUniform("ux_mouse", rot( lmx));
                shader.setUniform("uy_mouse", rot(-lmy));

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

                auto tick = app.getTicks();
                app.getLight()
                    .direction = glm::normalize(
                        glm::vec3(cos(tick*0.005), 
                        -sin(tick*0.005), 
                        0.75f
                    ));
            },
            [&](){
                auto [w, h] = app.getSimbolsSize();
                DirectBuffer buffer(w, h, 0.5625);
                buffer.shade(app.getMainShader());
                
                buffer.draw(app.getConsole());
            }
        );
    }

    app.finish();
}