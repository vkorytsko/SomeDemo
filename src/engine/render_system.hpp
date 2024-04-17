#pragma once

#include <windows.h>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <memory>


namespace SD::RENDER {
    class Renderer;
    class FrameBuffer;
}

namespace SD::ENGINE {

const DirectX::XMFLOAT3 EMPTY_COLOR = { 0.1f, 0.1f, 0.2f };

class RenderSystem
{
public:
    RenderSystem();
    ~RenderSystem();

    RenderSystem(RenderSystem&&) = default;
    RenderSystem& operator= (RenderSystem&&) = default;

    RenderSystem(RenderSystem const&) = delete;
    RenderSystem& operator= (RenderSystem const&) = delete;

    void Begin();
    void End();

    void BeginFrame();
    void EndFrame();

    void BeginImGui() const;
    void EndImGui() const;

    void OnWindowResize();
    void OnSpaceViewportResize(const float width, const float height);

    RENDER::Renderer* GetRenderer() const { return m_renderer.get(); }
    RENDER::FrameBuffer* GetFrameBuffer() const { return m_frameBuffer.get(); }

private:
    void InitImGui() const;
    void FiniImGui() const;

private:
    std::unique_ptr<RENDER::Renderer> m_renderer = nullptr;
    std::unique_ptr<RENDER::FrameBuffer> m_frameBuffer = nullptr;
};

}  // end namespace SD::RENDER
