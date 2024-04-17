#include "render_system.hpp"

#include "application.hpp"

#include <exceptions.hpp> 
#include <renderer.hpp>
#include <frame_buffer.hpp>
#include <debug_layer.hpp>

#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>


namespace SD::ENGINE {

RenderSystem::RenderSystem()
{
    const auto& window = ENGINE::Application::GetApplication()->GetWindow();
    const auto width = window->GetWidth();
    const auto height = window->GetHeight();
    const auto handel = window->GetHandle();

    m_renderer = std::make_unique<RENDER::Renderer>(width, height, handel);
    m_frameBuffer = std::make_unique<RENDER::FrameBuffer>(m_renderer.get(), width, height);

    D3D_DEBUG_LAYER(m_renderer.get());

    // gain access to texture subresource in swap chain (back buffer)
    Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
    D3D_THROW_INFO_EXCEPTION(m_renderer->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
    auto* rtv = m_renderer->GetRenderTargetView();
    m_renderer->GetDevice()->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &rtv);

    InitImGui();
}

RenderSystem::~RenderSystem()
{
    FiniImGui();
}

void RenderSystem::InitImGui() const
{
    const auto& window = ENGINE::Application::GetApplication()->GetWindow();

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    static const std::string config = SD_CFG_DIR + std::string("imgui.ini");

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = config.c_str();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // https://github.com/ocornut/imgui/issues/707#issuecomment-678611331
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    style.GrabRounding = style.FrameRounding = 2.3f;
    style.WindowRounding = 0.0f;

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(window->GetHandle());
    ImGui_ImplDX11_Init(m_renderer->GetDevice(), m_renderer->GetContext());
}

void RenderSystem::FiniImGui() const
{
    // Destroy backends
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
}

void RenderSystem::BeginImGui() const
{
    D3D_DEBUG_LAYER(m_renderer.get());

    auto* rtv = m_renderer->GetRenderTargetView();
    D3D_THROW_IF_INFO(m_renderer->GetContext()->OMSetRenderTargets(1u, &rtv, nullptr));


    // Start the Dear ImGui frame
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }
    
    // Start Docking
    {
        bool open = true;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGui::Begin("DockSpace", &open, windowFlags);

    	ImGui::PopStyleVar(2);

        // Submit the DockSpace
        const ImGuiID dockSpaceId = ImGui::GetID("DockSpace");
        const ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_None;
        ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), dockSpaceFlags);
    }

    const auto& window = ENGINE::Application::GetApplication()->GetWindow();
    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = window->GetWidth();
    vp.Height = window->GetHeight();
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    D3D_THROW_IF_INFO(m_renderer->GetContext()->RSSetViewports(1u, &vp));
}

void RenderSystem::EndImGui() const
{
    // End docking
    {
        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void RenderSystem::OnWindowResize()
{
    D3D_DEBUG_LAYER(m_renderer.get());

    D3D_THROW_IF_INFO(m_renderer->GetContext()->OMSetRenderTargets(0, nullptr, nullptr));

    m_renderer->GetRenderTargetView()->Release();

    // Preserve the existing buffer count and format.
    // Automatically choose the width and height to match the client rect for HWNDs.
    D3D_THROW_IF_INFO(m_renderer->GetSwapChain()->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

    // Get buffer and create a render-target-view.
    Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
    D3D_THROW_INFO_EXCEPTION(m_renderer->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
    auto* rtv = m_renderer->GetRenderTargetView();
    D3D_THROW_INFO_EXCEPTION(m_renderer->GetDevice()->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &rtv));
}

void RenderSystem::OnSpaceViewportResize(const float width, const float height)
{
    const auto& camera = ENGINE::Application::GetApplication()->GetCamera();

    m_frameBuffer->resize(m_renderer.get(), static_cast<UINT>(width), static_cast<UINT>(height));
    camera->onSpaceViewportResize();
}

void RenderSystem::Begin()
{
    D3D_DEBUG_LAYER(m_renderer.get());

    const float color1[] = { EMPTY_COLOR.x, EMPTY_COLOR.y, EMPTY_COLOR.z, 1.0f };
    D3D_THROW_IF_INFO(m_renderer->GetContext()->ClearRenderTargetView(m_frameBuffer->getRTV().Get(), color1));
    D3D_THROW_IF_INFO(m_renderer->GetContext()->ClearDepthStencilView(m_frameBuffer->getDSV().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u));

    const float color2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    D3D_THROW_IF_INFO(m_renderer->GetContext()->ClearRenderTargetView(m_renderer->GetRenderTargetView(), color2));
}

void RenderSystem::End()
{
    D3D_DEBUG_LAYER(m_renderer.get());

    debugLayer->Set();

    HRESULT hr;
    // First argument for VSync
    if (FAILED(hr = m_renderer->GetSwapChain()->Present(0u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw D3D_EXCEPTION(m_renderer->GetDevice()->GetDeviceRemovedReason());
        }
        else
        {
            throw D3D_EXCEPTION(hr);
        }
    }
}

void RenderSystem::BeginFrame()
{
    D3D_DEBUG_LAYER(m_renderer.get());

    D3D_THROW_IF_INFO(m_renderer->GetContext()->OMSetRenderTargets(1u, m_frameBuffer->getRTV().GetAddressOf(), m_frameBuffer->getDSV().Get()));

    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<float>(m_frameBuffer->width());
    vp.Height = static_cast<float>(m_frameBuffer->height());
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    D3D_THROW_IF_INFO(m_renderer->GetContext()->RSSetViewports(1u, &vp));
}

void RenderSystem::EndFrame()
{
}
}  // end namespace SD::ENGINE
