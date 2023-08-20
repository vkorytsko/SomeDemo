#include "renderer.hpp"

#include "application.hpp"
#include "exceptions.hpp"

#include "frame_buffer.hpp"

#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>


namespace SD::RENDER {

Renderer::Renderer()
    : m_debugLayer(std::make_unique<DebugLayer>())
{
    D3D_DEBUG_LAYER(this);

    const auto& window = ENGINE::Application::GetApplication()->GetWindow();
    const auto width = window->GetWidth();
    const auto height = window->GetHeight();

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = static_cast<UINT>(width);
    sd.BufferDesc.Height = static_cast<UINT>(height);
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = window->GetHandle();
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    UINT deviceFlags = 0u;
    if (m_debugLayer->isInitialised())
    {
        deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    }

    // create device and front/back buffers, and swap chain and rendering context
    D3D_THROW_INFO_EXCEPTION(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        deviceFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        m_pSwapChain.GetAddressOf(),
        m_pD3dDevice.GetAddressOf(),
        nullptr,
        m_pD3dContext.GetAddressOf()
    ));

    m_frameBuffer = std::make_unique<FrameBuffer>(this, width, height);

    // gain access to texture subresource in swap chain (back buffer)
    Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
    D3D_THROW_INFO_EXCEPTION(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf()));

    InitImGui();
}

Renderer::~Renderer()
{
    FiniImGui();
}

void Renderer::InitImGui() const
{
    const auto& window = ENGINE::Application::GetApplication()->GetWindow();

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(window->GetHandle());
    ImGui_ImplDX11_Init(m_pD3dDevice.Get(), m_pD3dContext.Get());
}

void Renderer::FiniImGui() const
{
    // Destroy backends
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
}

void Renderer::BeginImGui() const
{
    D3D_DEBUG_LAYER(this);

    D3D_THROW_IF_INFO(m_pD3dContext->OMSetRenderTargets(1u, m_pRenderTargetView.GetAddressOf(), nullptr));


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
    D3D_THROW_IF_INFO(m_pD3dContext->RSSetViewports(1u, &vp));
}

void Renderer::EndImGui() const
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

void Renderer::OnWindowResize()
{
    D3D_DEBUG_LAYER(this);

    const auto& window = ENGINE::Application::GetApplication()->GetWindow();
    const auto width = window->GetWidth();
    const auto height = window->GetHeight();

    D3D_THROW_IF_INFO(m_pD3dContext->OMSetRenderTargets(0, nullptr, nullptr));

    m_pRenderTargetView->Release();

    // Preserve the existing buffer count and format.
    // Automatically choose the width and height to match the client rect for HWNDs.
    D3D_THROW_IF_INFO(m_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

    // Get buffer and create a render-target-view.
    Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
    D3D_THROW_INFO_EXCEPTION(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf()));
}

void Renderer::OnSpaceViewportResize(const float width, const float height)
{
    const auto& camera = ENGINE::Application::GetApplication()->GetCamera();

    m_frameBuffer->resize(this, static_cast<UINT>(width), static_cast<UINT>(height));
    camera->onSpaceViewportResize();
}

void Renderer::Begin()
{
    D3D_DEBUG_LAYER(this);

    const float color1[] = { EMPTY_COLOR.x, EMPTY_COLOR.y, EMPTY_COLOR.z, 1.0f };
    D3D_THROW_IF_INFO(m_pD3dContext->ClearRenderTargetView(m_frameBuffer->getRTV().Get(), color1));
    D3D_THROW_IF_INFO(m_pD3dContext->ClearDepthStencilView(m_frameBuffer->getDSV().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u));

    const float color2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    D3D_THROW_IF_INFO(m_pD3dContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color2));
}

void Renderer::End()
{
    D3D_DEBUG_LAYER(this);

    debugLayer->Set();

    HRESULT hr;
    // First argument for VSync
    if (FAILED(hr = m_pSwapChain->Present(0u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw D3D_EXCEPTION(m_pD3dDevice->GetDeviceRemovedReason());
        }
        else
        {
            throw D3D_EXCEPTION(hr);
        }
    }
}

void Renderer::BeginFrame()
{
    D3D_DEBUG_LAYER(this);

    D3D_THROW_IF_INFO(m_pD3dContext->OMSetRenderTargets(1u, m_frameBuffer->getRTV().GetAddressOf(), m_frameBuffer->getDSV().Get()));

    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<float>(m_frameBuffer->width());
    vp.Height = static_cast<float>(m_frameBuffer->height());
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    D3D_THROW_IF_INFO(m_pD3dContext->RSSetViewports(1u, &vp));
}

void Renderer::EndFrame()
{
}

ID3D11Device* Renderer::GetDevice() const
{
    return m_pD3dDevice.Get();
}

ID3D11DeviceContext* Renderer::GetContext() const
{
    return m_pD3dContext.Get();
}

DebugLayer* Renderer::GetDebugLayer() const
{
    return m_debugLayer.get();
}

FrameBuffer* Renderer::GetFrameBuffer() const
{
    return m_frameBuffer.get();
}

}  // end namespace SD::RENDER
