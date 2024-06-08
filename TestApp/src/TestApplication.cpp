#include "TestApplication.h"

// Tests
#include "Tests/ModelViewer.h"
#include "Tests/OGLVK.h"
#include "Tests/QuadsTest.h"

namespace TestApp
{
    TestApplication::TestApplication()
    {
        auto frameworkSettings = pxl::FrameworkConfig::GetSettings();

        std::string windowTitle = "pxlFramework Test App";
        std::string buildType = "Unknown Build Type";
        std::string rendererAPIType = "Unknown Renderer API";
        pxl::RendererAPIType windowRendererAPI = frameworkSettings.RendererAPI;
        pxl::WindowMode windowMode = frameworkSettings.WindowMode;

        #ifdef TA_DEBUG
            buildType = "Debug x64";
        #elif TA_RELEASE
            buildType = "Release x64";
        #elif TA_DIST
            buildType = "Distribute x64";
        #endif

        switch (windowRendererAPI)
        {
            case pxl::RendererAPIType::None:
                rendererAPIType = "No Renderer API";
                break;
            case pxl::RendererAPIType::OpenGL:
                rendererAPIType = "OpenGL";
                break;
            case pxl::RendererAPIType::Vulkan:
                rendererAPIType = "Vulkan";
                break;
        }

        windowTitle = "pxlFramework Test App - " + buildType + " - " + rendererAPIType;

        pxl::WindowSpecs windowSpecs = {};
        windowSpecs.Width = 1600;
        windowSpecs.Height = 900;
        windowSpecs.Title = windowTitle;
        windowSpecs.RendererAPI = windowRendererAPI;
        windowSpecs.WindowMode = windowMode;

        #define TEST_NAME OGLVK

        m_OnStartFunc =  TEST_NAME::OnStart;
        m_OnUpdateFunc = TEST_NAME::OnUpdate;
        m_OnRenderFunc = TEST_NAME::OnRender;
        m_OnImGuiRender = TEST_NAME::OnImGuiRender;
        
        m_OnStartFunc(windowSpecs);
    }

    TestApplication::~TestApplication()
    {
        // Save framework settings // TODO: this should be semi-automatic and handled by the application class
        //auto frameworkSettings = pxl::FrameworkConfig::GetSettings();

        //frameworkSettings.WindowMode = m_Window->GetWindowMode();
        //frameworkSettings.RendererAPI // TODO

        //pxl::FrameworkConfig::SetSettings(frameworkSettings);
    }

    void TestApplication::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;
        
        m_OnUpdateFunc(dt);
    }

    void TestApplication::OnRender()
    {
        PXL_PROFILE_SCOPE;
        
        m_OnRenderFunc();
    }

    void TestApplication::OnGuiRender() // Function only gets called if ImGui is initialized
    {
        PXL_PROFILE_SCOPE;
        
        m_OnImGuiRender();
    }
}