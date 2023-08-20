#include "scene_browser_panel.hpp"

#include <imgui.h>


namespace
{
enum NodeID : uint64_t
{
	SceneOverview = 0,
	Hierarchy
};
} // end namespace

namespace SD::ENGINE {

void SceneBrowserPanel::Draw(World* world)
{
	ImGui::Begin("Scene Browser");

	DrawScenesOverview(world);
	DrawHierarchy(world->m_scenes[world->m_selectedScene].get());

	ImGui::End();
}

void SceneBrowserPanel::DrawScenesOverview(World* world)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	flags |= ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
	flags |= ImGuiTreeNodeFlags_DefaultOpen;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	if (ImGui::TreeNodeEx((void*)NodeID::SceneOverview, flags, "Scenes"))
	{
		const auto& selectedScene = world->m_scenes[world->m_selectedScene];

		if (ImGui::BeginCombo("Active Scene", selectedScene->m_name.c_str()))
		{
			for (const auto& scene : world->m_scenes)
			{
				const bool selected = selectedScene->m_id == scene->m_id;

				if (ImGui::Selectable(scene->m_name.c_str(), selected))
				{
					world->m_selectedScene = selectedScene->m_id;
				}

				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::TreePop();
	}
}

void SceneBrowserPanel::DrawHierarchy(const World::Scene* scene)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	flags |= ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
	flags |= ImGuiTreeNodeFlags_DefaultOpen;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	if (ImGui::TreeNodeEx((void*)NodeID::Hierarchy, flags, "Hierarchy"))
	{
		DrawNode(scene->m_root.get());

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
		{
			m_selectedNode = nullptr;
		}

		ImGui::TreePop();
	}
}

void SceneBrowserPanel::DrawNode(World::Node* node)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	if (node->m_children.empty())
	{
		flags |= ImGuiTreeNodeFlags_Bullet;
	}
	if (m_selectedNode && node->m_id == m_selectedNode->m_id)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	const bool expanded = ImGui::TreeNodeEx((void*)(uint64_t)node->m_id, flags, node->m_name.c_str());

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
	{
		m_selectedNode = node;
	}

	if (expanded)
	{
		for (const auto& child : node->m_children)
		{
			DrawNode(child.get());
		}
		ImGui::TreePop();
	}
}

} // end namespace SD::ENGINE
