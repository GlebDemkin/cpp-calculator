#pragma once

#include <windows.h>
#include <filesystem>

namespace calculator 
{

class SharedLibrary // Владеет загруженной DLL
{
public:
    explicit SharedLibrary(std::filesystem::path const& path);
    ~SharedLibrary();

    SharedLibrary(SharedLibrary const&) = delete;
    SharedLibrary& operator=(SharedLibrary const&) = delete;
    SharedLibrary(SharedLibrary&& other) noexcept;

    FARPROC symbol(char const* name) const; // FARPROC — тип указателя на функцию, возвращаемый GetProcAddress

private:
    /*
    HMODULE — тип Windows, представляющий дескриптор загруженного модуля
    Windows возвращает его при загрузке DLL, а мы передаём его в последующие вызовы для работы с этой библиотекой
    */
    HMODULE module_ = nullptr;
};

}
