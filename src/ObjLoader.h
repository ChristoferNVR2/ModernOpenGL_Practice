//
// Created by chrisvega on 10/27/24.
//

#pragma once

#include <vector>
#include <string>

struct Vec3 {
    float x, y, z;
};

struct Vec2 {
    float u, v;
};

struct Vertex {
    Vec3 position;
    Vec2 texCoord;
    Vec3 normal;
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
