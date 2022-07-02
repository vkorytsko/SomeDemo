#include "camera.hpp"

#include "application.hpp"


Camera::Camera()
{
	updateView();
	updateProjection();
}

void Camera::Rotate(float yaw, float pitch, float roll)
{
	m_rotation.x += yaw;
	m_rotation.y += pitch;
	m_rotation.z += roll;
	
	updateView();
}

void Camera::Translate(float x, float y, float z)
{
	const XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	XMVECTOR position = XMLoadFloat3(&m_position);
	XMVECTOR translation = XMVector3TransformCoord(XMVectorSet(x, y, z, 0.0f), camRotationMatrix);
	position += translation;

	XMStoreFloat3(&m_position, position);
}

const XMFLOAT3 Camera::getPosition() const
{
	return m_position;
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

void Camera::updateView()
{
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	const XMVECTOR position = XMLoadFloat3(&m_position);
	const XMVECTOR target = position + XMVector3TransformCoord(FORWARD, camRotationMatrix);
	const XMVECTOR up = XMVector3TransformCoord(UP, camRotationMatrix);

	m_view = XMMatrixLookAtLH(position, target, up);
}

void Camera::updateProjection()
{
	const auto& window = Application::GetApplication()->GetWindow();
	const float aspect = window.GetWidht() / window.GetHeight();
	m_projection = XMMatrixPerspectiveFovLH(FOV, aspect, NEAR_Z, FAR_Z);
}

