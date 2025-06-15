#pragma once
#include "ModelLoader.h"
#include "ShaderManager.h"
// parasoft-begin-suppress ALL "suppress all violations"
#include <d3d11.h>
#include <DirectXMath.h>
// parasoft-end-suppress ALL "suppress all violations"

#include "OBJModel.h"

// helper struct to simplify (non)collision detection
struct Gap
{
    float left;
    float right;

    Gap(float minX, float maxX) : left(minX), right(maxX) {}
};

struct ConstantBuffer
{
    XMMATRIX mWorld;
    float rotationAngle = 0.0f;
    bool enableRotation = false;
    bool enableTorch = false;
    float padding[2] = { 0.0f, 0.0f };
};

class GameObject {
private:
    DirectX::XMMATRIX _worldMatrix;
    CComPtr <ID3D11VertexShader> _vertexShader;
    CComPtr <ID3D11PixelShader> _pixelShader;
    CComPtr <ID3D11InputLayout> _vertexLayout;
    CComPtr <ID3D11Buffer> _vertexBuffer;
    CComPtr <ID3D11Buffer> _indexBuffer;
    CComPtr <ID3D11Buffer> _constantBuffer;

    CComPtr <ID3D11ShaderResourceView> _pColorTextureRV;
    CComPtr <ID3D11ShaderResourceView> _pNormalTextureRV;
    CComPtr <ID3D11ShaderResourceView> _pHeightTextureRV;
    CComPtr <ID3D11SamplerState> _pTextureSampler;
    std::vector<std::wstring> _texturePaths;
    std::vector<WORD> _indices;

    UINT _indexCount = 0;
    UINT _vertexCount = 0;

    DirectX::XMFLOAT3 _position = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 _rotation = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 _scale = { 1.0f, 1.0f, 1.0f };

protected:
    void bind(ID3D11DeviceContext* context, const UINT stride) const;
    void initResources(ID3D11Device* device, ID3D11DeviceContext* context, const D3D11_SUBRESOURCE_DATA& verticesInitData, const D3D11_SUBRESOURCE_DATA& indicesInitData, D3D_PRIMITIVE_TOPOLOGY topology, const D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutSize, UINT byteWidthVertices, UINT byteWidthIndices, const UINT stride, const std::wstring& shaderFile);
    void initNoTextureObject(ID3D11Device* device, ID3D11DeviceContext* context, const OBJModel& geometry, const std::wstring& shaderFile);
    void initTextureResources(ID3D11Device* device, ID3D11DeviceContext* context);
    void updateTextureResources(ID3D11DeviceContext* context, std::vector<UINT>& textureSlots, UINT samplerSlot) const;
public:
    GameObject() {
        updateWorldMatrix();
    }

    explicit GameObject(const std::vector<std::wstring>& texturePaths)
    {
        setTexturePaths(texturePaths);
		updateWorldMatrix();
    }

    virtual void init(ID3D11Device* device, ID3D11DeviceContext* context) = 0;
    virtual void render(ID3D11DeviceContext* context, float time = 0.0f) = 0;
    virtual void bindBuffers(ID3D11DeviceContext* context) const = 0;

    void updateWorldMatrix() {
        _worldMatrix = XMMatrixScaling(_scale.x, _scale.y, _scale.z) *
            XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z) *
            XMMatrixTranslation(_position.x, _position.y, _position.z);
    }

    const DirectX::XMMATRIX& getWorldMatrix() const { return _worldMatrix; }

    void setPosition(const DirectX::XMFLOAT3& position) {
        _position = position;
        updateWorldMatrix();
    }

    const XMFLOAT3& getPosition() const { return _position; }

    void setRotation(const DirectX::XMFLOAT3& rotation) {
        _rotation = rotation;
        updateWorldMatrix();
    }
    const XMFLOAT3& getRotation() const { return _rotation; }

    void setScale(const DirectX::XMFLOAT3& scale) {
        _scale = scale;
        updateWorldMatrix();
    }
    const XMFLOAT3& getScale() const { return _scale; }

    void setVertexShader(ID3D11VertexShader* vertexShader) { _vertexShader = vertexShader; }
    ID3D11VertexShader* getVertexShader() const { return _vertexShader; }

    void setPixelShader(ID3D11PixelShader* pixelShader) { _pixelShader = pixelShader; }
    ID3D11PixelShader* getPixelShader() const { return _pixelShader; }

    void setVertexLayout(ID3D11InputLayout* vertexLayout) { _vertexLayout = vertexLayout; }
    ID3D11InputLayout* getVertexLayout() const { return _vertexLayout; }

    void setVertexBuffer(ID3D11Buffer* vertexBuffer) { _vertexBuffer = vertexBuffer; }
    ID3D11Buffer* getVertexBuffer() const { return _vertexBuffer.p; }

    void setIndexBuffer(ID3D11Buffer* indexBuffer, UINT indexCount) { _indexBuffer = indexBuffer; _indexCount = indexCount; }
    ID3D11Buffer* getIndexBuffer() const { return _indexBuffer; }

    void setConstantBuffer(ID3D11Buffer* constantBuffer) { _constantBuffer = constantBuffer; }
    ID3D11Buffer* getConstantBuffer() const { return _constantBuffer; }

    const UINT getIndexCount() const { return _indexCount; }

    const UINT getVertexCount() const { return _vertexCount; }
    void setVertexCount(const UINT vertexCount) { _vertexCount = vertexCount; }

    void setTexturePaths(const std::vector<std::wstring>& texturePaths) { _texturePaths = texturePaths; }

    void setIndices(const std::vector<WORD>& indices) { _indices = indices; }
    const std::vector<WORD>& getIndices() const { return _indices; }

    void convertIndices(const std::vector<Face>& faces);
    static std::vector<D3D11_INPUT_ELEMENT_DESC> createLayout() {
        return {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
    }

    virtual ~GameObject();
};