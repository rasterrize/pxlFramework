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

        static void OnFileDrop(const std::vector<std::string>& paths);
        static void LoadMesh(const std::filesystem::path& path);
        static void AddModelToList(const std::string& modelName);
    };
}