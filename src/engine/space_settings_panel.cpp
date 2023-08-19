#include "space_settings_panel.hpp"

#include <string>

#include <imgui.h>


namespace SD::ENGINE {

void SpaceSettingsPanel::Draw(Space* space)
{
	ImGui::Begin("Space Settings");

	DrawSimulationSettings(space);

	ImGui::End();
}

void SpaceSettingsPanel::DrawSimulationSettings(Space* space)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	flags |= ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
	flags |= ImGuiTreeNodeFlags_DefaultOpen;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	const uint64_t id = 0;
	if (ImGui::TreeNodeEx((void*)id, flags, "Simulation"))
	{
		const std::string simulationTime = "Simulation Time: " + std::to_string(space->m_simulationTime);
		ImGui::Text(simulationTime.c_str());

		ImGui::DragFloat("Simulation Factor", &space->m_simulationFactor, 0.1f);

		if (ImGui::SmallButton(space->m_simulationPaused ? "Start" : "Pause"))
		{
			space->m_simulationPaused = !space->m_simulationPaused;
		}

		if (ImGui::SmallButton("Reset"))
		{
			space->m_simulationTime = 0.0f;
		}

		ImGui::TreePop();
	}
}

} // end namespace SD::ENGINE
