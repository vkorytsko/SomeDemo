#include "viewport_panel.hpp"

#include <imgui.h>

#include "application.hpp"
#include "frame_buffer.hpp"


namespace SD::ENGINE {

void ViewportPanel::Draw()
{
	const auto& app = Application::GetApplication();
	const auto& rendererSystem = app->GetRenderSystem();
	const auto& frameBuffer = rendererSystem->GetFrameBuffer();

	ImGuiWindowFlags flags = ImGuiWindowFlags_None;
	flags |= ImGuiWindowFlags_NoNavInputs;
	bool open = true;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport", &open, flags);

	const auto viewportSize = ImGui::GetContentRegionAvail();

	const auto width = static_cast<UINT>(viewportSize.x);
	const auto height = static_cast<UINT>(viewportSize.y);
	if (frameBuffer->width() != width || frameBuffer->height() != height)
	{
		rendererSystem->OnSpaceViewportResize(viewportSize.x, viewportSize.y);
	}

	ImGui::Image((void*)frameBuffer->getSRV().Get(), viewportSize);

	ImGui::End();
	ImGui::PopStyleVar();
}

} // end namespace SD::ENGINE
