#pragma once

#include <DirectXMath.h>



namespace SD::ENGINE {

const float FOV = DirectX::XM_PIDIV2;
const float NEAR_Z = 0.05f;
const float FAR_Z = 100.0f;
const float ROTATION_SPEED = 5.0f;
const float MOVEMENT_SPEED = 5.0f;


class Camera
{
public:
	Camera();
	~Camera() = default;

	void Update(float dt);

	const DirectX::XMFLOAT3 getPosition() const;
	const DirectX::XMFLOAT3 getDirection() const;
	const DirectX::XMFLOAT3 getRotation() const;

	const DirectX::XMMATRIX getView() const;
	const DirectX::XMMATRIX getProjection() const;

private:
	void Rotate(float yaw, float pitch);
	void Translate(float x, float y, float z);

	void updateView();
	void updateProjection();

private:
	DirectX::XMFLOAT3 m_position = {0.0f, 1.0f, -4.0f};
	DirectX::XMFLOAT3 m_rotation = {0.0f, 0.0f, 0.0f};

	DirectX::XMMATRIX m_view = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX m_projection = DirectX::XMMatrixIdentity();

	const DirectX::XMVECTOR FORWARD = {0.0f, 0.0f, 1.0f, 0.0f};
	const DirectX::XMVECTOR UP = {0.0f, 1.0f, 0.0f, 0.0f};
};

}  // end namespace SD::ENGINE
