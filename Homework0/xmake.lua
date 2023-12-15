set_project("GAMES101")
set_defaultarchs("windows|x64")

add_rules("plugin.compile_commands.autoupdate", { outputdir = ".vscode" })
-- add_rules("plugin.vsxmake.autoupdate")

add_rules("mode.release", "mode.debug")
set_defaultmode("release")

if is_plat("windows") then
    set_toolchains("msvc")
    set_runtimes("MT")
    add_defines("_CRT_SECURE_NO_WARNINGS")
    add_cxxflags("/Zc:__cplusplus", "/Zc:preprocessor")
elseif is_plat("linux") then
    set_toolchains("clang")
end

set_encodings("source:utf-8", "target:utf-8")
set_warnings("allextra")
set_languages("cxx20")

add_requires("vcpkg::eigen3",  { alias = "eigen" })

target("00", function()
    set_kind("binary")
    set_extension(".exe")
    set_default(true)

    add_files("src/*.cpp")
    add_packages("eigen")

    set_rundir("./")
    set_runargs()
end)
