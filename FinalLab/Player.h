#pragma once
#include "GameObject.h"

enum class PlayerState {
	MOVE,
	JUMP,
	IDLE,
	FALL,
	DEAD
};

enum class LookDirection {
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Player : public GameObject
{
private:
	OBJModel _geometry;  // Shared geometry for all player parts

	void bindBuffers(ID3D11DeviceContext* context) const override;

	PlayerState _state = PlayerState::IDLE;
	LookDirection _lookDirection = LookDirection::RIGHT;

	float _moveSpeed = 5.0f;

	bool _lookVertical = false;
	float _verticalAngle = 0.0f;

	bool _renderable = true;

	// to manage movement, jumping and gravity
	XMFLOAT3 _velocity = { 0.0f, 0.0f, 0.0f };
	const float _gravity = -9.8f;
	const float _maxFallSpeed = -50.0f;
	float _initialJumpY = 5.0f;
	float _maxJumpY = 3.0f;
	float _initialJumpX = 4.0f;
	float _groundLevel = -0.5f;
	float _visibleY = -10.0f;

	bool torchEnabled = false;

public:
	explicit Player();
	Player(const Player&) = delete;
	Player(Player&&) = delete;
	Player& operator=(const Player&) = delete;
	Player& operator=(Player&&) = delete;

	const OBJModel& getGeometry() const { return _geometry; }

	void init(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void render(ID3D11DeviceContext* context, float time = 0.0f) override;

	void reset();

	void setState(PlayerState state) { _state = state; }
	const PlayerState getState() const { return _state; }

	void applyGravity(float deltaTime);
	void jump();

	void setLookDirection(LookDirection direction) { _lookDirection = direction; }
	const LookDirection getLookDirection() const { return _lookDirection; }

	const float getMoveSpeed() const { return _moveSpeed; }

	void setLookVertical(bool vertical) { _lookVertical = vertical; }
	void setVerticalAngle(float angle) { _verticalAngle = angle; }

	void incrementJumpY(float increment) {
		_initialJumpY += increment;
		if (_initialJumpY > 10.0f) _initialJumpY = 10.0f;
		else if (_initialJumpY < 0.0f) _initialJumpY = 0.0f;
	}

	bool isInGap(const std::vector<std::shared_ptr<Gap>>& gaps) const;
	bool getRenderable() const { return _renderable; }

	bool isTorchEnabled() const { return torchEnabled; }
	void setTorchEnabled(bool enabled) { torchEnabled = enabled; }
	void toggleTorch() { torchEnabled = !torchEnabled; }

	~Player();
};