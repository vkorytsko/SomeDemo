#pragma once

#include <wrl.h>
#include <vector>
#include <string>
#include <dxgidebug.h>

class DebugLayer
{
public:
	DebugLayer();
	~DebugLayer() = default;

	DebugLayer(const DebugLayer&) = delete;
	DebugLayer& operator=(const DebugLayer&) = delete;

	void Set() noexcept;
	std::vector<std::string> GetMessages() const;

private:
	unsigned long long m_start = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> m_pDxgiInfoQueue;
};