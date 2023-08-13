#pragma once

#include <memory>

#include "timer.hpp"

#include "world.hpp"
#include "debug_world.hpp"


namespace SD::ENGINE {

class Space
{
public:
	Space();
	~Space() = default;

	void Init();
    void Destroy();

	void Simulate(float dt);
	void Update(float dt);
	void Draw();
    
private:
    // Common
    std::unique_ptr<Timer> m_pTimer;
    float m_simulationTime = 0.0f;

    // Worlds
    std::unique_ptr<World> m_pWorld;
    std::unique_ptr<DebugWorld> m_pDebugWorld;
};
}  // end namespace SD::ENGINE
