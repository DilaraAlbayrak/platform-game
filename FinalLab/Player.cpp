#include "Player.h"

Player::Player()
{
    reset();

    _geometry = ModelLoader::loadPlayerModel();

    convertIndices(_geometry.faces);
    _geometry.indices = getIndices();
}

void Player::reset()
{
    _renderable = true;
    auto transformData = ConfigParser::getInstance()->getTransform("PlayerTransform");
    assert(transformData.size() == 3 && "Player Transform must include position, rotation, and scale!");

    setPosition(transformData[0]);
    setRotation(transformData[1]);
    setScale(transformData[2]);
    updateWorldMatrix();
}

void Player::applyGravity(float deltaTime) {
    const XMFLOAT3 pos = getPosition();

    if (_state == PlayerState::JUMP) {
        if (pos.y >= _maxJumpY) {
            setPosition(XMFLOAT3(pos.x, _maxJumpY, pos.z));
            _velocity.y += _gravity * deltaTime;
        }
    }
    else if (_state == PlayerState::DEAD) {
        _velocity.y += _gravity * deltaTime;
		_velocity.x *= 0.1f;
        if (_velocity.y < _maxFallSpeed) {
            _velocity.y = _maxFallSpeed;
        }
    }
    else if (pos.y <= _groundLevel) // _groundLevel is a buffer
    {
        _velocity.y = 0.0f;
        _velocity.x = 0.0f;
    }
    else {
        _velocity.y += _gravity * deltaTime;

        if (_velocity.y < _maxFallSpeed) {
            _velocity.y = _maxFallSpeed;
        }
    }

    float newY = pos.y + _velocity.y * deltaTime;
    if (_state != PlayerState::DEAD)
        newY = (newY < _groundLevel) ? _groundLevel : newY;
    const float newX = pos.x + _velocity.x * deltaTime;

    setPosition(XMFLOAT3(newX, newY, pos.z));
    updateWorldMatrix();
}


void Player::jump()
{
    if (_state != PlayerState::JUMP) {
        _state = PlayerState::JUMP;
        _velocity.y = _initialJumpY;
        if (_lookDirection == LookDirection::RIGHT) { _initialJumpX = 4.0f; }
        else { _initialJumpX = -4.0f; }
        _velocity.x = _initialJumpX;
    }
}

void Player::init(ID3D11Device* device, ID3D11DeviceContext* context)
{
	initNoTextureObject(device, context, _geometry, L"Player.fx");
}

void Player::render(ID3D11DeviceContext* context, float time)
{
    //std::string stateString = playerStateToString(_state);
    //OutputDebugStringA(("Player State: " + stateString + "\n").c_str());
    // print player position

      //  OutputDebugStringA(("Player Position: " + std::to_string(getPosition().x) + " " + std::to_string(getPosition().y) + " " + std::to_string(getPosition().z) + "\n").c_str());

    const ConstantBuffer cbPlayer{
        DirectX::XMMatrixTranspose(getWorldMatrix()),
        _verticalAngle,
        _lookVertical,
        torchEnabled
    };

    ID3D11Buffer* const constantBuffer = getConstantBuffer();
    context->UpdateSubresource(constantBuffer, 0, nullptr, &cbPlayer, 0, 0);
    context->VSSetConstantBuffers(1, 1, &constantBuffer); // register b1

    // Set the primitive topology
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->VSSetShader(getVertexShader(), nullptr, 0);
    context->PSSetShader(getPixelShader(), nullptr, 0);
    bindBuffers(context);
    context->DrawIndexed(getIndexCount(), 0, 0);

    if (_state == PlayerState::MOVE)
    {
        float xMove = getPosition().x;

        if (_lookDirection == LookDirection::RIGHT) xMove += time * _moveSpeed;
        else if (_lookDirection == LookDirection::LEFT) xMove -= time * _moveSpeed;
        setPosition(XMFLOAT3(xMove, getPosition().y, getPosition().z));
    }

    applyGravity(time);

    if (_state == PlayerState::DEAD && getPosition().y <= _visibleY)
    {
        _renderable = false;
    }
}

void Player::bindBuffers(ID3D11DeviceContext* context) const
{
    const UINT stride = sizeof(SimpleVertex);
    bind(context, stride);
}

bool Player::isInGap(const std::vector<std::shared_ptr<Gap>>& gaps) const
{
    const float playerX = getPosition().x;
    const float playerY = getPosition().y;
    const float buffer = 0.7f;

    // Binary search: Gaps are assumed to be sorted by 'left'
    auto it = std::lower_bound(gaps.begin(), gaps.end(), playerX, [](const std::shared_ptr<Gap>& gap, float x) {
        return gap->right < x; // Search for the first gap where right >= x
        });

    // Check the current gap
    if (it != gaps.end() && (*it)->left < playerX - buffer && playerX < (*it)->right - buffer && fabs(playerY - _groundLevel) < 0.1) {
        return true; // Player is in the current gap
    }

    // Check the previous gap (to handle cases when playerX is slightly less than the current gap's left boundary)
    if (it != gaps.begin()) {
        --it; // Move to the previous gap
        if ((*it)->left > playerX + buffer && playerX > (*it)->right + buffer && fabs(playerY - _groundLevel) < 0.1) {
            return true; // Player is in the previous gap
        }
    }

    return false; // Player is not in any gap
}

Player::~Player() = default;

