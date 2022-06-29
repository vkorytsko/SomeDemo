#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <exception>

#include "application.hpp"
#include "exceptions.hpp"


int main(int, char**)
{
    try
    {
        return Application().Run();
    }
    catch (const SomeException& e)
    {
        MessageBoxW(nullptr, e.w_what(), L"Some Exception", MB_ICONERROR | MB_DEFAULT_DESKTOP_ONLY);
    }
    catch (const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "Standard Exception", MB_ICONERROR | MB_DEFAULT_DESKTOP_ONLY);
    }
    catch (...)
    {
        MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_ICONERROR | MB_DEFAULT_DESKTOP_ONLY);
    }

    return -1;
}
