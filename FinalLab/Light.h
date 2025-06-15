#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "ConfigParser.h"

enum class LightType { Ambient, Directional, Spot };

class Light final {
private:
    LightType _type;
    LightBuffer _buffer;
    CComPtr <ID3D11Buffer> _constantBuffer;

public:
    explicit Light(LightType type, const LightBuffer& buffer)
        : _type(type), _buffer(buffer) {
    }

    LightType getType() const { return _type; }
    void setType(LightType type) { _type = type; }

    const LightBuffer& getBuffer() const { return _buffer; }
    void setBuffer(const LightBuffer& buffer) { _buffer = buffer; }

    const ID3D11Buffer* getConstantBuffer() const { return _constantBuffer; }
    void setConstantBuffer(ID3D11Buffer* constantBuffer) { _constantBuffer = constantBuffer; }

    void init(ID3D11Device* device);
    void render(ID3D11DeviceContext* context, UINT registerSlot) const;
};