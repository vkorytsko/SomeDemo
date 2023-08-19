#include "node_properties_panel.hpp"

#include <imgui.h>
#include <imgui_internal.h>


namespace SD::ENGINE {

DirectX::XMFLOAT3 ToEulerAngles(const DirectX::XMFLOAT4 quaternion)
{
	float xx = quaternion.x * quaternion.x;
	float yy = quaternion.y * quaternion.y;
	float zz = quaternion.z * quaternion.z;

	float m31 = 2.f * quaternion.x * quaternion.z + 2.f * quaternion.y * quaternion.w;
	float m32 = 2.f * quaternion.y * quaternion.z - 2.f * quaternion.x * quaternion.w;
	float m33 = 1.f - 2.f * xx - 2.f * yy;

	float cy = sqrtf(m33 * m33 + m31 * m31);
	float cx = atan2f(-m32, cy);
	if (cy > 16.f * FLT_EPSILON)
	{
		float m12 = 2.f * quaternion.x * quaternion.y + 2.f * quaternion.z * quaternion.w;
		float m22 = 1.f - 2.f * xx - 2.f * zz;

		return DirectX::XMFLOAT3(cx, atan2f(m31, m33), atan2f(m12, m22));
	}
	else
	{
		float m11 = 1.f - 2.f * yy - 2.f * zz;
		float m21 = 2.f * quaternion.x * quaternion.y - 2.f * quaternion.z * quaternion.w;

		return DirectX::XMFLOAT3(cx, 0.f, atan2f(-m21, m11));
	}
}

DirectX::XMFLOAT4 ToQuaternion(const DirectX::XMFLOAT3& angles)
{
	DirectX::XMFLOAT4 q;
	DirectX::XMStoreFloat4(&q, DirectX::XMQuaternionRotationRollPitchYaw(angles.x, angles.y, angles.z));

	return q;
}

DirectX::XMFLOAT3 ToRadians(const DirectX::XMFLOAT3& angles)
{
	return { DirectX::XMConvertToRadians(angles.x), DirectX::XMConvertToRadians(angles.y) , DirectX::XMConvertToRadians(angles.z) };
}

DirectX::XMFLOAT3 ToDegrees(const DirectX::XMFLOAT3& angles)
{
	return { DirectX::XMConvertToDegrees(angles.x), DirectX::XMConvertToDegrees(angles.y) , DirectX::XMConvertToDegrees(angles.z) };
}

void DrawVector3Control(const std::string& label, DirectX::XMFLOAT3& vector, const DirectX::XMFLOAT3& defaultVector)
{
	ImGui::PushID(label.c_str());

	ImGui::Columns(2);

	{
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(label.c_str());
	}
	
	ImGui::NextColumn();

	{
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
			if (ImGui::Button("X", buttonSize))
			{
				vector.x = defaultVector.x;
			}
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##X", &vector.x, 0.1f);
			ImGui::PopItemWidth();
		}

		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
			ImGui::SameLine();
			if (ImGui::Button("Y", buttonSize))
			{
				vector.y = defaultVector.y;
			}
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Y", &vector.y, 0.1f);
			ImGui::PopItemWidth();
		}

		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
			ImGui::SameLine();
			if (ImGui::Button("Z", buttonSize))
			{
				vector.z = defaultVector.z;
			}
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Z", &vector.z, 0.1f);
			ImGui::PopItemWidth();
		}

		ImGui::PopStyleVar();
	}

	ImGui::Columns(1);

	ImGui::PopID();
}

void NodePropertiesPanel::Draw(World::Node* node)
{
	ImGui::Begin("Node Properties");

	if (node)
	{
		DrawTransform(node);

		DrawMesh(node->m_mesh.get());
	}

	ImGui::End();
}

void NodePropertiesPanel::DrawTransform(World::Node* node)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	flags |= ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
	flags |= ImGuiTreeNodeFlags_DefaultOpen;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	const uint64_t id = 0;
	if (ImGui::TreeNodeEx((void*)id, flags, "Transform"))
	{
		DirectX::XMVECTOR scale;
		DirectX::XMVECTOR rotation;
		DirectX::XMVECTOR translation;
		DirectX::XMMatrixDecompose(&scale, &rotation, &translation, node->m_localTransform);

		// Scale
		{
			DirectX::XMFLOAT3 s, os;
			DirectX::XMStoreFloat3(&s, scale);
			DirectX::XMStoreFloat3(&os, node->m_originalScale);
			DrawVector3Control("Scale", s, os);
			scale = DirectX::XMLoadFloat3(&s);
		}

		// Rotation
		{
			DirectX::XMFLOAT4 q, oq;
			DirectX::XMStoreFloat4(&q, rotation);
			DirectX::XMStoreFloat4(&oq, node->m_originalRotation);
			DirectX::XMFLOAT3 r = ToDegrees(ToEulerAngles(q));
			DirectX::XMFLOAT3 or = ToDegrees(ToEulerAngles(oq));
			DrawVector3Control("Rotation", r, or);
			q = ToQuaternion(ToRadians(r));
			rotation = DirectX::XMLoadFloat4(&q);
		}

		//Translation
		{
			DirectX::XMFLOAT3 t, ot;
			DirectX::XMStoreFloat3(&t, translation);
			DirectX::XMStoreFloat3(&ot, node->m_originalTranslation);
			DrawVector3Control("Translation", t, ot);
			translation = DirectX::XMLoadFloat3(&t);
		}

		node->m_localTransform = DirectX::XMMatrixAffineTransformation(
			scale,
			{ 0.f, 0.f, 0.f, 0.f },
			rotation,
			translation);

		ImGui::TreePop();
	}
}

void NodePropertiesPanel::DrawMesh(const World::Mesh* mesh)
{
	if (!mesh)
	{
		return;
	}

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	flags |= ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
	flags |= ImGuiTreeNodeFlags_DefaultOpen;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	const uint64_t id = 1;
	if (ImGui::TreeNodeEx((void*)id, flags, "Mesh"))
	{
		ImGui::Text(mesh->m_name.c_str());

		uint64_t primId = 0;
		for (const auto& primitive : mesh->m_primitives)
		{
			DrawPrimitive(++primId, primitive.get());
		}

		ImGui::TreePop();
	}
}

void NodePropertiesPanel::DrawPrimitive(const uint64_t id, const World::Primitive* primitive)
{
	if (!primitive)
	{
		return;
	}

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	flags |= ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
	flags |= ImGuiTreeNodeFlags_DefaultOpen;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	const std::string name = "Primitive " + std::to_string(id);
	if(ImGui::TreeNodeEx((void*)id, flags, name.c_str()))
	{
		// TODO
		ImGui::Text("Primitive properties");

		DrawMaterial(primitive->m_material.get());

		ImGui::TreePop();
	}
}

void NodePropertiesPanel::DrawMaterial(const World::Material* material)
{
	if (!material)
	{
		return;
	}

	// TODO

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	flags |= ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
	flags |= ImGuiTreeNodeFlags_DefaultOpen;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	const std::string name = "Material";
	const uint64_t id = 0;
	if (ImGui::TreeNodeEx((void*)id, flags, "Material"))
	{
		// TODO
		ImGui::Text("Material properties");

		ImGui::TreePop();
	}
}

} // end namespace SD::ENGINE
