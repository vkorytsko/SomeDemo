#pragma once

#include <DirectXMath.h>


using namespace DirectX;


const float FOV = XM_PI / 2.0f;
const float NEAR_Z = 0.5f;
const float FAR_Z = 100.0f;


class Camera
{
public:
	Camera();
	~Camera() = default;

	void Rotate(float yaw, float pitch, float roll = 0.0f);
	void Translate(float x, float y, float z);

	const XMFLOAT3 getPosition() const;
	const XMFLOAT3 getRotation() const;

	const XMMATRIX getView() const;
	const XMMATRIX getProjection() const;

private:
	void updateView();
	void updateProjection();

private:
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;

	XMFLOAT3 m_position = {0.0f, 0.0f, 0.0f};
	XMFLOAT3 m_rotation = {0.0f, 0.0f, 0.0f};

	XMMATRIX m_view = XMMatrixIdentity();
	XMMATRIX m_projection = XMMatrixIdentity();

	const XMVECTOR FORWARD = {0.0f, 0.0f, 1.0f, 0.0f};
	const XMVECTOR UP = {0.0f, 1.0f, 0.0f, 0.0f};
};
