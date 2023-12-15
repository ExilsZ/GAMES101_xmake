if is_plat("windows") then
    set_toolchains("msvc")
    add_cxxflags("/Zc:__cplusplus", "/Zc:preprocessor")
elseif is_plat("linux") then
    set_toolchains("clang")
end

set_encodings("source:utf-8", "target:utf-8")
set_warnings("allextra")
set_languages("cxx20")

target("Hello World 你好", function()
    set_kind("binary")
    set_extension(".exe")
    set_default(true)
    add_files("*.cpp")
end)
