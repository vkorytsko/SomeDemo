#pragma once

#include <exception>
#include <string>
#include <vector>
#include <Windows.h>


class SomeException : public std::exception
{
public:
    SomeException(int line, const char* file) noexcept;

    const wchar_t* w_what() const noexcept;

    const std::wstring& GetFilename() const noexcept;
    int GetLine() const noexcept;

    virtual const std::wstring GetInfoMessage() const noexcept;
    virtual const std::wstring GetType() const noexcept;

private:
    const int m_line;
    const std::wstring m_filename;

    mutable std::wstring m_whatBuffer;
};


class SomeWinException : public SomeException
{
public:
    SomeWinException(int line, const char* file, HRESULT hr) noexcept;

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
    SomeD3DException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;

    const std::wstring GetInfoMessage() const noexcept final;
    const std::wstring GetType() const noexcept final;

    std::wstring GetErrorInfo() const noexcept;

private:
    std::wstring m_errorInfo;
};

// Win Exceptions Macro
#define WIN_THROW_IF_FAILED(hrcall) {HRESULT hr = (hrcall); if(FAILED(hr)) throw SomeWinException(__LINE__, __FILE__, hr);}
#define WIN_THROW_LAST_EXCEPTION() {throw  SomeWinException(__LINE__, __FILE__, GetLastError());}


// D3D Exceptions Macro
#define D3D_THROW_NOINFO_EXCEPTION(hrcall) if(FAILED(hr = (hrcall))) throw SomeD3DException(__LINE__, __FILE__, hr)

#ifndef NDEBUG
#define D3D_EXCEPTION(hr) SomeD3DException(__LINE__, __FILE__, hr, debugLayer.GetMessages())
#define D3D_THROW_INFO_EXCEPTION(hrcall) debugLayer.Set(); if(FAILED(hr = (hrcall))) throw D3D_EXCEPTION(hr)
#define GFX_THROW_IF_INFO(call) debugLayer.Set(); (call); {auto msgs = debugLayer.GetMessages(); if(!msgs.empty()) {throw SomeD3DException(__LINE__, __FILE__, S_OK, msgs);}}
#else
#define D3D_EXCEPTION(hr) SomeD3DException(__LINE__, __FILE__, hr)
#define D3D_THROW_INFO_EXCEPTION(hrcall) D3D_THROW_NOINFO_EXCEPTION(hrcall)
#define GFX_THROW_IF_INFO(call) (call)
#endif
