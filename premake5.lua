workspace "Minecraft-Clone"
    platforms { "Win32" , "Win64" }
    configurations { "Debug" , "Release" }
    startproject "Minecraft-Clone"


filter { "platforms:Win32" }
    system "Windows"
    architecture "x86"

filter { "platforms:Win64" }
    system "Windows"
    architecture "x86_64"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "Olala/vendor/imgui"
    include "Olala/vendor/yaml-cpp"
group ""


include "Olala"
include "Minecraft-Clone"

    