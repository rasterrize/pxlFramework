#pragma once

#include <pxl/pxl.h>

namespace TestApp
{
    class ModelViewer
    {
    public:
        static void OnStart(pxl::WindowSpecs& windowSpecs);
        static void OnUpdate(float dt);
        static void OnRender();
        static void OnGUIRender();
        static void OnClose();
        static std::shared_ptr<pxl::Window> GetWindow();

        static void LoadMesh(const std::filesystem::path& path);
        static void AddModelToList(const std::string& modelName);

        static void OnWindowPathDropEvent(pxl::WindowPathDropEvent& e);
        static void OnWindowResizeEvent(pxl::WindowResizeEvent& e);
        static void OnKeyDownEvent(pxl::KeyDownEvent& e);
    };
}