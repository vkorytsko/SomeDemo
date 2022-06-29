#include "debug_layer.hpp"

#include <dxgidebug.h>
#include <memory>


#include "exceptions.hpp"

#pragma comment(lib, "dxguid.lib")

const DXGI_INFO_QUEUE_MESSAGE_SEVERITY MIN_SEVERITY_LEVEL = DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR;
//const DXGI_INFO_QUEUE_MESSAGE_SEVERITY MIN_SEVERITY_LEVEL = DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING;


DebugLayer::DebugLayer()
{
	// define function signature of DXGIGetDebugInterface
	typedef HRESULT(WINAPI* LPDXGIGETDEBUGINTERFACE)(REFIID, void**);

	// load the dll that contains the function DXGIGetDebugInterface
	const auto dxgiDebug = LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (dxgiDebug == nullptr)
	{
		WIN_THROW_LAST_EXCEPTION();
	}

	// get address of DXGIGetDebugInterface in dll
	const auto dxgiGetDebugInterface = reinterpret_cast<LPDXGIGETDEBUGINTERFACE>(
		reinterpret_cast<void*>(GetProcAddress(dxgiDebug, "DXGIGetDebugInterface"))
		);
	if (dxgiGetDebugInterface == nullptr)
	{
		WIN_THROW_LAST_EXCEPTION();
	}

	HRESULT hr;
	D3D_THROW_NOINFO_EXCEPTION(dxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &m_pDxgiInfoQueue));
}

void DebugLayer::Set() noexcept
{
	// set the index (next) so that the next all to GetMessages()
	// will only get errors generated after this call
	m_start = m_pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DebugLayer::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = m_pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = m_start; i < end; i++)
	{
		HRESULT hr;
		SIZE_T messageLength;
		// get the size of message i in bytes
		D3D_THROW_NOINFO_EXCEPTION(m_pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		// allocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		// get the message and push its description into the vector
		D3D_THROW_NOINFO_EXCEPTION(m_pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		if (pMessage->Severity <= MIN_SEVERITY_LEVEL) {
			messages.emplace_back(pMessage->pDescription);
		}
	}
	return messages;
}
