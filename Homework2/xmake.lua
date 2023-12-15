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
    add_syslinks("Gdi32", "MSImg32", "User32", "shell32", "OpenGL32", "AdvAPI32", "ComDlg32")
elseif is_plat("linux") then
    set_toolchains("clang")
end

set_encodings("source:utf-8", "target:utf-8")
set_warnings("allextra")
set_languages("cxx20")

add_requires("vcpkg::opencv4",  { alias = "opencv4" })
add_requires("vcpkg::libpng",  { alias = "libpng" })
add_requires("vcpkg::libwebp",  { alias = "libwebp" })
add_requires("vcpkg::libjpeg-turbo",  { alias = "libjpeg-turbo" })
add_requires("vcpkg::liblzma",  { alias = "liblzma" })
add_requires("vcpkg::tiff",  { alias = "tiff" })
add_requires("vcpkg::zlib ",  { alias = "zlib" })

target("02", function()
    set_kind("binary")
    set_extension(".exe")
    set_default(true)
    add_files("src/*.cpp")

    add_packages("eigen3", "opencv4")
    add_packages("libpng", "libwebp", "libjpeg-turbo", "liblzma", "tiff", "zlib")

    set_rundir("./")
    set_runargs()
end)
