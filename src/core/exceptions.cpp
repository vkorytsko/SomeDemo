#include "exceptions.hpp"

#include <sstream>
#include <comdef.h>

#include "utils.hpp"


namespace SD {

SomeException::SomeException(int line, const wchar_t* file, const std::wstring message) noexcept
	: m_line(line)
	, m_filename(file)
	, m_message(message)
{
}

const wchar_t* SomeException::w_what() const noexcept
{
	m_whatBuffer = GetInfoMessage();

	return m_whatBuffer.c_str();
}

const std::wstring SomeException::GetInfoMessage() const noexcept
{
	std::wostringstream woss;
	woss << "[Type] " << GetType() << std::endl
		<< "[File] " << GetFilename() << std::endl
		<< "[Line] " << GetLine() << std::endl;

	if (!m_message.empty())
	{
		woss << "[Message]" << m_message << std::endl;
	}

	return woss.str();
}

const std::wstring SomeException::GetType() const noexcept
{
	return L"Some Exception";
}

const std::wstring& SomeException::GetFilename() const noexcept
{
	return m_filename;
}

int SomeException::GetLine() const noexcept
{
	return m_line;
}


SomeWinException::SomeWinException(int line, const wchar_t* file, HRESULT hr) noexcept
	: SomeException(line, file)
	, m_hresult(hr)
{
}

const std::wstring SomeWinException::GetInfoMessage() const noexcept
{
	std::wostringstream woss;
	woss << SomeException::GetInfoMessage() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;

	return woss.str();
}

const std::wstring SomeWinException::GetType() const noexcept
{
	return L"Some Win Exception";
}

const std::wstring SomeWinException::GetErrorCode() const noexcept
{
	std::wostringstream woss;
	woss << std::hex << "0x" << std::uppercase << m_hresult
		<< std::dec << " (" << static_cast<unsigned long>(m_hresult) << ")"
		<< std::endl;

	return woss.str();
}

const std::wstring SomeWinException::GetErrorDescription() const noexcept
{
	return _com_error(m_hresult).ErrorMessage();
}


SomeD3DException::SomeD3DException(int line, const wchar_t* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	: SomeWinException(line, file, hr)
{
	for (const auto& msg : infoMsgs) {
		m_errorInfo += AToWstring(msg);
		m_errorInfo.push_back('\n');
	}
}

const std::wstring SomeD3DException::GetInfoMessage() const noexcept
{
	std::wostringstream woss;
	woss << SomeWinException::GetInfoMessage() << std::endl;

	if (!m_errorInfo.empty()) {
		woss << "[Error Info] " << GetErrorInfo() << std::endl;
	}

	return woss.str();
}

const std::wstring SomeD3DException::GetType() const noexcept
{
	return L"Some D3D Exception";
}

std::wstring SomeD3DException::GetErrorInfo() const noexcept
{
	return m_errorInfo;
}

}  // end namespace SD
