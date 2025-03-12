#include <CL/opencl.hpp>
#include <iostream>
#include <vector>

int main() {
    // Шаг 1: Инициализация платформы и устройств
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) {
        std::cerr << "No OpenCL platforms found!" << std::endl;
        return -1;
    }
    cl::Platform platform = platforms[0];
    std::cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if (devices.empty()) {
        std::cerr << "No OpenCL devices found!" << std::endl;
        return -1;
    }
    cl::Device device = devices[0];
    std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;

    // Шаг 2: Создание контекста и очереди команд
    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    // Шаг 3: Компиляция kernel-кода
    const char* kernelSource = R"(
    __kernel void vectorAdd(__global const float* a, __global const float* b, __global float* result) {
        int id = get_global_id(0);
        result[id] = a[id] + b[id];
    }
    )";
    cl::Program::Sources sources;
    sources.push_back({kernelSource, strlen(kernelSource)});
    cl::Program program(context, sources);
    if (program.build({device}) != CL_SUCCESS) {
        std::cerr << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
        return -1;
    }

    // Шаг 4: Подготовка данных
    size_t size = 10;
    std::vector<float> a(size, 1.0f), b(size, 2.0f), result(size);
    cl::Buffer bufferA(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * size, a.data());
    cl::Buffer bufferB(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * size, b.data());
    cl::Buffer bufferResult(context, CL_MEM_WRITE_ONLY, sizeof(float) * size);

    // Шаг 5: Выполнение kernel
    cl::Kernel kernel(program, "vectorAdd");
    kernel.setArg(0, bufferA);
    kernel.setArg(1, bufferB);
    kernel.setArg(2, bufferResult);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(size), cl::NullRange);
    queue.finish();

    // Шаг 6: Чтение результатов
    queue.enqueueReadBuffer(bufferResult, CL_TRUE, 0, sizeof(float) * size, result.data());
    std::cout << "Result: ";
    for (float val : result) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    return 0;
}