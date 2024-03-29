#include "camera.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <algorithm>
#include <cmath>

#include "application.hpp"
#include "frame_buffer.hpp"


namespace SD::ENGINE {

using namespace DirectX;

Camera::Camera()
{
	updateView();
	updateProjection();
}

void Camera::Update(float dt)
{
	const auto& app = Application::GetApplication();
	if (!app->IsActive() || !app->IsCameraActive())
	{
		return;
	}

	const auto& window = app->GetWindow();

	XMFLOAT3 translation = { 0.0f, 0.0f, 0.0f };
	const float movement = dt * MOVEMENT_SPEED;
	if (GetKeyState('W') < 0) {
		translation.z += movement;
	}
	if (GetKeyState('S') < 0) {
		translation.z -= movement;
	}
	if (GetKeyState('A') < 0) {
		translation.x -= movement;
	}
	if (GetKeyState('D') < 0) {
		translation.x += movement;
	}
	if (GetKeyState('Q') < 0) {
		translation.y -= movement;
	}
	if (GetKeyState('E') < 0) {
		translation.y += movement;
	}
	Translate(translation.x, translation.y, translation.z);

	POINT cursorPos;
	GetCursorPos(&cursorPos);
	POINT centerPos = window->GetCenter();
	const float yaw = (cursorPos.x - centerPos.x) * dt * ROTATION_SPEED;
	const float pitch = (cursorPos.y - centerPos.y) * dt * ROTATION_SPEED;
	Rotate(yaw, pitch);

	updateView();

	window->CenterCursor();
}

void Camera::Rotate(float yaw, float pitch)
{
	m_rotation.x += yaw;
	m_rotation.y += pitch;

	m_rotation.x = std::fmodf(m_rotation.x, XM_2PI);
	m_rotation.y = std::clamp(m_rotation.y, -XM_PIDIV2 + 0.01f, XM_PIDIV2 - 0.01f);
}

void Camera::Translate(float x, float y, float z)
{
	const XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.y, m_rotation.x, m_rotation.z);

	XMVECTOR position = XMLoadFloat3(&m_position);
	XMVECTOR translation = XMVector3TransformCoord(XMVectorSet(x, y, z, 0.0f), camRotationMatrix);
	position += translation;

	XMStoreFloat3(&m_position, position);
}

const XMFLOAT3 Camera::getPosition() const
{
	return m_position;
}

const XMFLOAT3 Camera::getDirection() const
{
	XMFLOAT4 direction;
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.y, m_rotation.x, m_rotation.z);
	XMStoreFloat4(&direction, XMVector3TransformCoord(FORWARD, camRotationMatrix));

	return { direction.x, direction.y, direction.z };
}

const XMFLOAT3 Camera::getRotation() const
{
	return m_rotation;
}

const XMMATRIX Camera::getView() const
{
	return m_view;
}

const XMMATRIX Camera::getProjection() const
{
	return m_projection;
}

void Camera::onSpaceViewportResize()
{
	updateProjection();
}

void Camera::updateView()
{
	const XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.y, m_rotation.x, m_rotation.z);

	const XMVECTOR position = XMLoadFloat3(&m_position);
	const XMVECTOR target = position + XMVector3TransformCoord(FORWARD, camRotationMatrix);

	m_view = XMMatrixLookAtLH(position, target, UP);
}

void Camera::updateProjection()
{
	const auto& app = Application::GetApplication();
	const auto& renderer = app->GetRenderer();
	const auto& frameBuffer = renderer->GetFrameBuffer();

	const float aspect = static_cast<float>(frameBuffer->width()) / static_cast<float>(frameBuffer->height());
	m_projection = XMMatrixPerspectiveFovLH(FOV, aspect, NEAR_Z, FAR_Z);
}

}  // end namespace SD::ENGINE
