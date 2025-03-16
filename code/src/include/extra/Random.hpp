#pragma once

#include <random>
#include <vector>

std::random_device RANDOM_DEVICE;
std::mt19937 gen(RANDOM_DEVICE());

namespace eml{
    template<class T = int>
    T randint(T a, T b){
        std::uniform_int_distribution<> dist(a,b);
        return dist(gen);
    }

    float uniform(float a, float b){
        std::uniform_real_distribution<> dist(a,b);
        return dist(gen);    
    }

    template<class T>
    T choice(std::vector<T> vc){
        return vc[randint(0, vc.size()-1)];
    }

    template<class T>
    std::vector<T> vecrand(int size, T min_ampl, T max_ampl){
        std::vector<T> out;
        for(int i = 0; i < size; i++){
            if(min_ampl != max_ampl) out.push_back(uniform(min_ampl, max_ampl));
            else {out.push_back(0);}
        }
        return {out};
    }
};