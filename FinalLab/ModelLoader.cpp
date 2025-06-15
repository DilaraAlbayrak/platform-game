#include "ModelLoader.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include <array>  
#include <unordered_map>
#include <iostream>
#include <random>

OBJModel ModelLoader::loadPlayerModel() {
    // Get the player model path from ConfigParser
	const std::string playerModelPath = ConfigParser::getInstance()->getModelPath("PlayerDir.player_model");
    assert(!playerModelPath.empty() && "Player model path is empty!");

    // Parse the .obj file
	return parseOBJ(playerModelPath).at(0);
}

OBJModel ModelLoader::loadTreeModel() {
    // Get the player model path from ConfigParser
    const std::string treeModelPath = ConfigParser::getInstance()->getModelPath("TreeDir.tree_model");
    assert(!treeModelPath.empty() && "Tree model path is empty!");

    // Parse the .obj file
    return parseOBJ(treeModelPath, true).at(0);
}

std::vector<OBJModel> ModelLoader::loadBuildingModel() {
    const std::string buildingModelPath = ConfigParser::getInstance()->getModelPath("BuildingDir.building_model");
    assert(!buildingModelPath.empty() && "Building model path is empty!");

    std::vector<OBJModel> models = ModelLoader::parseOBJ(buildingModelPath);
    int modelIndex = 0;

    for (auto& model : models) {
        auto calculateTangentBinormal = [](const TexturedVertex& v0, const TexturedVertex& v1, const TexturedVertex& v2) {
            XMFLOAT3 edge1 = { v1.Pos.x - v0.Pos.x, v1.Pos.y - v0.Pos.y, v1.Pos.z - v0.Pos.z };
            XMFLOAT3 edge2 = { v2.Pos.x - v0.Pos.x, v2.Pos.y - v0.Pos.y, v2.Pos.z - v0.Pos.z };
            XMFLOAT2 deltaUV1 = { v1.TexCoord.x - v0.TexCoord.x, v1.TexCoord.y - v0.TexCoord.y };
            XMFLOAT2 deltaUV2 = { v2.TexCoord.x - v0.TexCoord.x, v2.TexCoord.y - v0.TexCoord.y };

            float det = deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x;
            if (fabs(det) < 1e-6f) det = 1.0f; // Divide-by-zero protection
            float invDet = 1.0f / det;

            XMFLOAT3 tangent = {
                invDet * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                invDet * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                invDet * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
            };

            XMFLOAT3 binormal = {
                invDet * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
                invDet * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
                invDet * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z)
            };

            return std::make_pair(tangent, binormal);
            };

        std::unordered_map<size_t, WORD> vertexIndexMap;

        auto hashVertex = [](const TexturedVertex& v) {
            size_t h1 = std::hash<float>()(v.Pos.x) ^ std::hash<float>()(v.Pos.y) ^ std::hash<float>()(v.Pos.z);
            size_t h2 = std::hash<float>()(v.TexCoord.x) ^ std::hash<float>()(v.TexCoord.y);
            size_t h3 = std::hash<float>()(v.Normal.x) ^ std::hash<float>()(v.Normal.y) ^ std::hash<float>()(v.Normal.z);
            return h1 ^ h2 ^ h3;
            };

        std::vector<TexturedVertex> vertices;
        std::vector<WORD> indices;

        for (size_t faceIndex = 0; faceIndex < model.faces.size(); ++faceIndex) {
            const auto& face = model.faces[faceIndex];

            std::array<TexturedVertex, 3> faceVertices;
            for (int i = 0; i < 3; ++i) {
                if (face.vertexIndices[i] >= model.vertices.size() ||
                    face.textureIndices[i] >= model.texCoords.size() ||
                    face.normalIndices[i] >= model.normals.size()) {

                    std::cerr << "Model Index: " << modelIndex << ", Face Index: " << faceIndex
                        << "\nVertex Index: " << face.vertexIndices[i]
                        << ", Texture Index: " << face.textureIndices[i]
                        << ", Normal Index: " << face.normalIndices[i]
                        << "\nVertices.size: " << model.vertices.size()
                        << ", TexCoords.size: " << model.texCoords.size()
                        << ", Normals.size: " << model.normals.size() << "\n";
                    continue;
                }

                const auto& position = model.vertices[face.vertexIndices[i]].pos;
                const auto& texCoord = model.texCoords[face.textureIndices[i]].uv;
                const auto& normal = model.normals[face.normalIndices[i]].dir;

                faceVertices[i].Pos = { position.x, position.y, position.z };
                faceVertices[i].TexCoord = { texCoord.x, texCoord.y };
                faceVertices[i].Normal = { normal.x, normal.y, normal.z };
            }

            auto tangentBinormal = calculateTangentBinormal(faceVertices[0], faceVertices[1], faceVertices[2]);
            const XMFLOAT3 tangent = tangentBinormal.first;
            const XMFLOAT3 binormal = tangentBinormal.second;

            for (auto& vertex : faceVertices) {
                vertex.Tangent = tangent;
                vertex.Binormal = binormal;

                const size_t hash = hashVertex(vertex);
                if (vertexIndexMap.find(hash) == vertexIndexMap.end()) {
                    vertexIndexMap[hash] = static_cast<WORD>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(vertexIndexMap[hash]);
            }
        }

        model.texturedVertices = vertices;
        model.indices = indices;
        ++modelIndex;
    }

    return models;
}

void ModelLoader::randomizeModelColors(OBJModel& model)
{
	const XMFLOAT4 colour = generateRandomColor();
	for (auto& vertex : model.vertices) {
        vertex.colour = colour;
	}
}

std::vector<OBJModel> ModelLoader::parseOBJ(const std::string& filePath, bool colourFill) {
    std::vector<OBJModel> models;
    OBJModel currentModel;
    std::ifstream file(filePath);
    assert(file.is_open() && "Failed to open OBJ file!");

    const XMFLOAT4 colour = generateRandomColor();

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string prefix;
        lineStream >> prefix;

        if (prefix == "o") {
            if (!currentModel.vertices.empty() || !currentModel.faces.empty()) {
                models.push_back(currentModel);
                currentModel = OBJModel();
            }
        }
        else if (prefix == "v") {
            SimpleVertex vertex;
            lineStream >> vertex.pos.x >> vertex.pos.y >> vertex.pos.z;
            if (colourFill) { vertex.colour = colour; }
            currentModel.vertices.push_back(vertex);
        }
        else if (prefix == "vt") {
            TexCoord texCoord;
            lineStream >> texCoord.uv.x >> texCoord.uv.y;
            currentModel.texCoords.push_back(texCoord);
        }
        else if (prefix == "vn") {
            Normal normal;
            lineStream >> normal.dir.x >> normal.dir.y >> normal.dir.z;
            currentModel.normals.push_back(normal);
        }
        else if (prefix == "f") {
            Face face;
            std::string vertex;
            while (lineStream >> vertex) {
                std::istringstream vertexStream(vertex);
                std::string index;
                int vIdx = 0, vtIdx = 0, vnIdx = 0;

                if (std::getline(vertexStream, index, '/')) {
                    vIdx = std::stoi(index);
                }
                if (std::getline(vertexStream, index, '/')) {
                    if (!index.empty()) {
                        vtIdx = std::stoi(index);
                    }
                }
                if (std::getline(vertexStream, index)) {
                    if (!index.empty()) {
                        vnIdx = std::stoi(index);
                    }
                }

                face.vertexIndices.push_back(vIdx - 1);
                if (vtIdx > 0) {
                    face.textureIndices.push_back(vtIdx - 1);
                }
                if (vnIdx > 0) {
                    face.normalIndices.push_back(vnIdx - 1);
                }

                if (vnIdx > 0 && vIdx > 0) {
                    const int vertexIndex = vIdx - 1;
                    const int normalIndex = vnIdx - 1;
                    currentModel.vertices[vertexIndex].normal = currentModel.normals[normalIndex].dir;
                }
            }
            currentModel.faces.push_back(face);
        }
    }

    if (!currentModel.vertices.empty() || !currentModel.faces.empty()) {
        models.push_back(currentModel);
    }

    return models;
}

DirectX::XMFLOAT4 ModelLoader::generateRandomColor(const DirectX::XMFLOAT3& minRGB, const DirectX::XMFLOAT3& maxRGB)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> rDist(minRGB.x, maxRGB.x);
    std::uniform_real_distribution<float> gDist(minRGB.y, maxRGB.y);
    std::uniform_real_distribution<float> bDist(minRGB.z, maxRGB.z);

    // Generate random RGB values and normalize to 0-1
    const float r = rDist(gen) / 255.0f;
    const float g = gDist(gen) / 255.0f;
    const float b = bDist(gen) / 255.0f;

    // Return as XMFLOAT4
    return DirectX::XMFLOAT4(r, g, b, 1.0f);
}