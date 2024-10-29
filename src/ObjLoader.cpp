//
// Created by chrisvega on 10/27/24.
//

#include "ObjLoader.h"

#include <iostream>
#include <fstream>
#include <sstream>

bool ObjLoader::Load(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }

    std::vector<Vec3> positions;
    std::vector<Vec2> texCoords;
    std::vector<Vec3> normals;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vec3 position{};
            iss >> position.x >> position.y >> position.z;
            positions.push_back(position);
        } else if (prefix == "vt") {
            Vec2 texCoord{};
            iss >> texCoord.u >> texCoord.v;
            texCoords.push_back(texCoord);
        } else if (prefix == "vn") {
            Vec3 normal{};
            iss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (prefix == "f") {
            unsigned int posIndex[3], texIndex[3], normIndex[3];
            char slash;
            for (int i = 0; i < 3; ++i) {
                iss >> posIndex[i] >> slash >> texIndex[i] >> slash >> normIndex[i];
                Vertex vertex{};
                vertex.position = positions[posIndex[i] - 1];
                vertex.texCoord = texCoords[texIndex[i] - 1];
                vertex.normal = normals[normIndex[i] - 1];
                vertex.print();
                vertices.push_back(vertex);
                indices.push_back(vertices.size() - 1);
            }
        }
    }

    file.close();

    m_Vertices = vertices;
    m_Indices = indices;

    return true;
}

const std::vector<Vertex>& ObjLoader::GetVertices() const {
    return m_Vertices;
}

const std::vector<unsigned int>& ObjLoader::GetIndices() const {
    return m_Indices;
}