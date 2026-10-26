#include "calculator/SharedLibrary.hpp"

#include <stdexcept>
#include <string>

namespace calculator 
{

/*
LoadLibraryW() просит Windows загрузить DLL в процесс
Буква W обозначает вариант, работающий с широкими строками Unicode
Это позволяет использовать пути с кириллицей
*/

SharedLibrary::SharedLibrary(std::filesystem::path const& path) : module_(LoadLibraryW(path.c_str())) 
{
    if (module_ == nullptr) 
    {
        throw std::runtime_error("Не удалось загрузить DLL: " + path.u8string()); // path.u8string() преобразует путь в строку UTF-8
    }
}

SharedLibrary::~SharedLibrary() 
{
    if (module_ != nullptr) 
    {
        FreeLibrary(module_);
    }
}

// После передачи владения старый объект не должен освобождать DLL
SharedLibrary::SharedLibrary(SharedLibrary&& other) noexcept : module_(other.module_) 
{
    other.module_ = nullptr;
}

FARPROC SharedLibrary::symbol(char const* name) const 
{
    auto address = GetProcAddress(module_, name);
    if (address == nullptr) 
    {
        throw std::runtime_error(std::string("В DLL нет функции: ") + name);
    }
    return address;
}

}
