#pragma once

#include "space.hpp"


namespace SD::ENGINE {

class SpaceSettingsPanel
{
public:
	SpaceSettingsPanel() = default;
	~SpaceSettingsPanel() = default;

	void Draw(Space* world);

private:
	void DrawSimulationSettings(Space* world);
};

} // end namespace SD::ENGINE
