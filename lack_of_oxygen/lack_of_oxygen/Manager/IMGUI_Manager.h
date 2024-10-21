
#pragma once
#ifndef LOF_IMGUI_MANAGER_H
#define LOF_IMGUI_MANAGER_H

 // Macros for accessing manager singleton instances
#define ECSM lof::IMGUI_Manager::get_instance()

 // Include header file
#include "Manager.h"

// Include other necessary headers
#include "../IMGUI/imgui.h"
#include "../IMGUI/imgui_impl_glfw.h"
#include "../IMGUI/imgui_impl_opengl3.h"

namespace lof {

    class IMGUI_Manager : public Manager {
    private:
        IMGUI_Manager();

        IMGUI_Manager(const IMGUI_Manager&) = delete;
        IMGUI_Manager& operator=(const IMGUI_Manager&) = delete;

    public:
        ;
    };

} // namespace lof

#endif // LOF_IMGUI_MANAGER_H
