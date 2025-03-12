#include <ConsoleAPI/capi.hpp>
#include <ConsoleAPI/raytracing/shaders.hpp>

Shader shader;

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

    auto &cam = shader.getCamera();
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
    console.hide_cursor();
    while(true){
        console.clear();
        for (int y = 0; y < console.height; ++y) {
            for (int x = 0; x < console.width; ++x) {
                console.pixel(x, y, pixel(
                    (float)x / console.width,
                    (float)y / console.height,
                    console.width / console.height * termAspect
                ));
            }
        }

        console.draw();
        usleep(dt);
        tick++;
    }
}