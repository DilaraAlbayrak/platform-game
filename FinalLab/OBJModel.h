#pragma once
// parasoft-begin-suppress ALL "suppress all violations"
#include <DirectXMath.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <vector>
// parasoft-end-suppress ALL "suppress all violations"

using namespace DirectX;

struct SimpleVertex
{
    XMFLOAT3 pos;
    XMFLOAT4 colour = { 0.4f, 0.3f, 0.4f, 1.0f };
	XMFLOAT3 normal;
};

struct TexturedVertex
{
    XMFLOAT3 Pos;
    XMFLOAT2 TexCoord;
    XMFLOAT3 Normal;
    XMFLOAT3 Tangent;
    XMFLOAT3 Binormal;
};

struct TexCoord
{
	XMFLOAT2 uv;
};

struct Normal 
{
	XMFLOAT3 dir;
};

struct Face {
    std::vector<int> vertexIndices;
    std::vector<int> textureIndices;
    std::vector<int> normalIndices;
};

struct OBJModel {
    std::vector<SimpleVertex> vertices;
	std::vector<TexturedVertex> texturedVertices;
    std::vector<TexCoord> texCoords;
    std::vector<Normal> normals;
    std::vector<Face> faces;
	std::vector<WORD> indices;
};
