#pragma once
#include <string>

namespace colors{
    struct foregroundDataset{
        const std::string reset = "\033[0m";
        const std::string black = "\033[30m";
        const std::string red = "\033[31m";
        const std::string green = "\033[32m";
        const std::string yellow = "\033[33m";
        const std::string blue = "\033[34m";
        const std::string purple = "\033[35m";
        const std::string cyan = "\033[36m";
        const std::string white = "\033[37m";
    } Fore;

    struct backgroundDataset{
        const std::string reset = "\033[0m";
        const std::string black = "\033[40m";
        const std::string red = "\033[41m";
        const std::string green = "\033[42m";
        const std::string yellow = "\033[43m";
        const std::string blue = "\033[44m";
        const std::string purple = "\033[45m";
        const std::string cyan = "\033[46m";
        const std::string white = "\033[47m";
    } Back;

    struct lightForegroundDataset{
        const std::string reset = "\033[0m";
        const std::string black = "\033[90m";
        const std::string red = "\033[91m";
        const std::string green = "\033[92m";
        const std::string yellow = "\033[93m";
        const std::string blue = "\033[94m";
        const std::string purple = "\033[95m";
        const std::string cyan = "\033[96m";
        const std::string gray = "\033[97m";
    } LightFore;

    struct lightBackgroundDataset{
        const std::string reset = "\033[0m";
        const std::string black = "\033[100m";
        const std::string red = "\033[101m";
        const std::string green = "\033[102m";
        const std::string yellow = "\033[103m";
        const std::string blue = "\033[104m";
        const std::string purple = "\033[105m";
        const std::string cyan = "\033[106m";
        const std::string gray = "\033[107m";
    } LightBack;
}