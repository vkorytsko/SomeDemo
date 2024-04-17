#pragma once

#include <exception>
#include <string>
#include <vector>
#include <Windows.h>


namespace SD {

class SomeException : public std::exception
{
public:
    SomeException(int line, const wchar_t* file, const std::wstring message = L"") noexcept;

    const wchar_t* w_what() const noexcept;

    const std::wstring& GetFilename() const noexcept;
    int GetLine() const noexcept;

    virtual const std::wstring GetInfoMessage() const noexcept;
    virtual const std::wstring GetType() const noexcept;

private:
    const int m_line;
    const std::wstring m_filename;
    const std::wstring m_message;

    mutable std::wstring m_whatBuffer;
};


class SomeWinException : public SomeException
{
public:
    SomeWinException(int line, const wchar_t* file, HRESULT hr) noexcept;

    const std::wstring GetInfoMessage() const noexcept override;
    const std::wstring GetType() const noexcept override;

    const std::wstring GetErrorCode() const noexcept;
    const std::wstring GetErrorDescription() const noexcept;

private:
    HRESULT m_hresult;
};


class SomeD3DException : public SomeWinException
{
public:
    SomeD3DException(int line, const wchar_t* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;

    const std::wstring GetInfoMessage() const noexcept final;
    const std::wstring GetType() const noexcept final;

    std::wstring GetErrorInfo() const noexcept;

private:
    std::wstring m_errorInfo;
};


#define THROW_SOME_EXCEPTION(msg) {throw SomeException(__LINE__, __FILEW__, msg);}

    // Win Exceptions Macro
#define WIN_THROW_IF_FAILED(hrcall) {HRESULT hr = (hrcall); if(FAILED(hr)) throw SomeWinException(__LINE__, __FILEW__ , hr);}
#define WIN_THROW_LAST_EXCEPTION() {throw  SomeWinException(__LINE__, __FILEW__ , GetLastError());}


// D3D Exceptions Macro
#define D3D_DEBUG_LAYER(renderer) auto debugLayer = renderer->GetDebugLayer()
#define D3D_EXCEPTION(hr) SomeD3DException(__LINE__, __FILEW__ , hr, debugLayer->GetMessages())
#define D3D_THROW_INFO_EXCEPTION(hrcall) {debugLayer->Set(); HRESULT hr = (hrcall); if(FAILED(hr)) throw D3D_EXCEPTION(hr);}
#define D3D_THROW_NOINFO_EXCEPTION(hrcall) {HRESULT hr = (hrcall); if(FAILED(hr)) throw SomeD3DException(__LINE__, __FILEW__ , hr);}
#define D3D_THROW_IF_INFO(call) debugLayer->Set(); (call); {auto msgs = debugLayer->GetMessages(); if(!msgs.empty()) {throw SomeD3DException(__LINE__, __FILEW__ , S_OK, msgs);}}

}  // end namespace SD
