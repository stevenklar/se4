-- Prepare some dynamic variables
PROJECT = "SniperElite4"

include "variables.lua"
include "functions.lua"
include "workspace.lua"

project(PROJECT)
    -- 64bit
    kind "SharedLib"

    includeIcetrix()
    includeProject()

    files {
        PROJECT_VENDOR .. "imgui/examples/imgui_impl_win32.*",
        PROJECT_VENDOR .. "imgui/examples/imgui_impl_dx11.*",
    } 

