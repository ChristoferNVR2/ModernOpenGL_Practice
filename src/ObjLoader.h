//
// Created by chrisvega on 10/27/24.
//

#pragma once

#include <vector>
#include <string>
#include <iostream>

struct Vec3 {
    float x, y, z;

    void print() const {
        std::cout<<"(x: "<<x<<" y: "<<y<<" z: "<<z<< ")" << std::endl;
    }
};

struct Vec2 {
    float u, v;

    void print() const {
        std::cout<<"(u: "<<u<<" v: "<<v<< ")" << std::endl;
    }
};

struct Vertex {
    Vec3 position;
    Vec2 texCoord;
    Vec3 normal;

    void print() const {
        std::cout << "pos: ";
        position.print();
        std::cout << "texture: ";
        texCoord.print();
        std::cout << "normal: ";
        normal.print();
    }
};

class ObjLoader {
public:
    bool Load(const std::string& filePath);
    const std::vector<Vertex>& GetVertices() const;
    const std::vector<unsigned int>& GetIndices() const;

private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
};
