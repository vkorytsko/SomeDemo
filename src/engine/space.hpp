#pragma once

#include <memory>

#include "timer.hpp"


namespace SD::ENGINE {

class World;
class DebugWorld;
class ViewportPanel;

class Space
{
private:
    friend class SpaceSettingsPanel;

public:
	Space();
	~Space();

	void Init();
    void Destroy();

	void Simulate(float dt);
	void Update(float dt);
	void DrawFrame();
	void DrawImGui();

    float simulationTime() const { return m_simulationTime; }
    
private:
    // Common
    std::unique_ptr<Timer> m_pTimer;

    float m_simulationTime = 0.0f;
    float m_simulationFactor = 1.0f;
    bool m_simulationPaused = false;

    // Worlds
    std::unique_ptr<World> m_pWorld = nullptr;
    std::unique_ptr<DebugWorld> m_pDebugWorld = nullptr;

    // Panels
    std::unique_ptr<SpaceSettingsPanel> m_spaceSettingsPanel = nullptr;
    std::unique_ptr<ViewportPanel> m_viewportPanel = nullptr;
};
}  // end namespace SD::ENGINE
