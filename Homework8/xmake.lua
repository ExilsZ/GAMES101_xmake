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

add_requires("glew", { alias = "glew" })
add_requires("glfw", { alias = "glfw" })
add_requires("freetype", { alias = "freetype" })
add_requires("libpng", { alias = "libpng" })
add_requires("brotli", { alias = "brotli" })
add_requires("bzip2", { alias = "bzip2" })

target("wingetopt", function()
    set_kind("static")
    add_files("include/wingetopt/*.c")
end)

target("CGL", function()
    set_kind("static")
    add_packages("glew", "glfw", "freetype")
    add_files("include/CGL/src/*.c")
    add_files("include/CGL/src/*.cpp")
end)

target("08", function()
    set_kind("binary")
    set_extension(".exe")
    set_default(true)
    add_files("src/*.cpp")

    add_packages("libpng", "brotli", "bzip2")

    add_deps("CGL")
    add_includedirs("include/CGL/include/")

    add_deps("wingetopt")
    add_includedirs("include/wingetopt")

    set_rundir("./")
    set_runargs("-s 128")
end)
