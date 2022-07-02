#pragma once

#include <DirectXMath.h>


using namespace DirectX;


const float FOV = XM_PIDIV2;
const float NEAR_Z = 0.5f;
const float FAR_Z = 100.0f;
const float ROTATION_SPEED = 10.0f;
const float MOVEMENT_SPEED = 10.0f;


class Camera
{
public:
	Camera();
	~Camera() = default;

	void Update(float dt);

	const XMFLOAT3 getPosition() const;
	const XMFLOAT3 getRotation() const;

	const XMMATRIX getView() const;
	const XMMATRIX getProjection() const;

private:
	void Rotate(float yaw, float pitch);
	void Translate(float x, float y, float z);

	void updateView();
	void updateProjection();

private:
	XMFLOAT3 m_position = {0.0f, 0.0f, 0.0f};
	XMFLOAT3 m_rotation = {0.0f, 0.0f, 0.0f};

	XMMATRIX m_view = XMMatrixIdentity();
	XMMATRIX m_projection = XMMatrixIdentity();

	const XMVECTOR FORWARD = {0.0f, 0.0f, 1.0f, 0.0f};
	const XMVECTOR UP = {0.0f, 1.0f, 0.0f, 0.0f};
};
