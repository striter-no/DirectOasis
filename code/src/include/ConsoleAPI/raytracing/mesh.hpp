#pragma once

#include <utils/files.hpp>
#include <utils/vector.hpp>

#include "init.hpp"
#include "figures.hpp"

enum MESH_TYPE {
    VERTEX_NORMALS,
    VERTEX_TEXTURES,
    VERTEX_NORMALS_TEXTURES,
    ONLY_VERTEXES
};

struct MTriangle {
    glm::vec3 v0, v1, v2;
    glm::vec3 normal;
    glm::vec2 uv0, uv1, uv2;

    Triangle raycast_form;

    Triangle getRayCastingTri(const Material &material, const glm::vec3 &color) const {
        return {v0, v1, v2, material, color};
    }
};

class Mesh: public Figure {
        MESH_TYPE type;

        glm::vec3 triNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
            glm::vec3 ab = b - a, ac = c - a;
            return glm::normalize(glm::cross(ab, ac));
        }

    public:
        std::vector<MTriangle> triangles;
        Material material;
        glm::vec3 color;

        void load(std::string path, bool addNormals = false){

            std::vector<glm::vec3> vertexes, normals;
            std::vector<glm::vec2> uvertexes;

            for(std::string line : utils::vec::stripsplit(utils::fls::getFile(path), '\n')){
                std::vector<std::string> splitted = utils::vec::split(line);
                
                if(splitted[0]=="v"){
                    float x = std::stof(splitted[1]), y = std::stof(splitted[2]), z = std::stof(splitted[3]);
                    vertexes.push_back({x, y, z});
                } else if(splitted[0]=="vt"){
                    float x = std::stof(splitted[1]), y = std::stof(splitted[2]);
                    uvertexes.push_back({x, y});
                } else if(splitted[0]=="vn"){
                    float x = std::stof(splitted[1]), y = std::stof(splitted[2]), z = std::stof(splitted[3]);
                    normals.push_back({x, y, z});
                } else if(splitted[0]=="f"){
                    int a,b,c,na,nb,nc,ua,ub,uc;
                    a = std::stoi(utils::vec::split(splitted[1], '/')[0]);
                    b = std::stoi(utils::vec::split(splitted[2], '/')[0]);
                    c = std::stoi(utils::vec::split(splitted[3], '/')[0]);
                    if(uvertexes.size()>0){
                        ua = std::stoi(utils::vec::split(splitted[1], '/')[1]);
                        ub = std::stoi(utils::vec::split(splitted[2], '/')[1]);
                        uc = std::stoi(utils::vec::split(splitted[3], '/')[1]);}
                    
                    if(normals.size()>0){
                        na = std::stoi(utils::vec::split(splitted[1], '/')[2]);
                        nb = std::stoi(utils::vec::split(splitted[2], '/')[2]);
                        nc = std::stoi(utils::vec::split(splitted[3], '/')[2]);}
                    
                    glm::vec3 enormal;
                    if(addNormals && !normals.size()>0){
                        enormal = triNormal(vertexes[a-1], vertexes[b-1], vertexes[c-1]);
                    }
                    
                    MTriangle tri;
                    tri.v0 = vertexes[a-1]; tri.v1 = vertexes[b-1]; tri.v2 = vertexes[c-1];
                    if(normals.size()>0) {tri.normal = normals[na-1];}
                    else if(addNormals){tri.normal = enormal;}
                    if(uvertexes.size()>0) {tri.uv0 = uvertexes[ua-1]; tri.uv1 = uvertexes[ub-1]; tri.uv2 = uvertexes[uc-1];}
                    
                    tri.raycast_form = tri.getRayCastingTri(material, color);
                    triangles.push_back(tri);
                }
            }
            bool isnormals = normals.size()>0;
            bool istexs = uvertexes.size()>0;

            if(isnormals && !istexs)      type = VERTEX_NORMALS;
            else if(!isnormals && istexs) type = VERTEX_TEXTURES;
            else if(isnormals && istexs)  type = VERTEX_NORMALS_TEXTURES;
            else                          type = ONLY_VERTEXES;
        }

        void intersect(Ray& ray) const;

        Mesh(std::string path, Material material, glm::vec3 color):
            material(material), color(color) {
            load(path, true);
        }

        Mesh(){}
        ~Mesh(){}
};