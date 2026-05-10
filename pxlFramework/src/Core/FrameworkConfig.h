#pragma once

#include "IniConfig.h"
#include "Renderer/GraphicsAPI.h"
#include "Renderer/Renderer.h"
#include "Window.h"

#ifdef PXL_DEBUG
    #define PXL_FRAMEWORK_CONFIG_FILE_NAME "Framework-dev.ini"
#else
    #define PXL_FRAMEWORK_CONFIG_FILE_NAME "Framework.ini"
#endif

namespace pxl
{
    enum class FrameworkSetting
    {
        WindowMode,
        WindowedWidth,
        WindowedHeight,
        WindowedX,
        WindowedY,
        FullscreenMonitor,
        FullscreenWidth,
        FullscreenHeight,
        FullscreenRefreshRate,
        GraphicsAPI,
        VerticalSync,
        AllowTearing,
        TripleBuffering,
        FramerateMode,
        CustomFramerateLimit,
        UnfocusedFramerateLimit,

#ifdef PXL_ENABLE_DEBUG_OVERLAY
        ShowDebugOverlay,
#endif
    };

    class FrameworkConfig : public IniConfig<FrameworkSetting>
    {
    public:
        FrameworkConfig()
            : IniConfig(PXL_FRAMEWORK_CONFIG_FILE_NAME)
        {
            // Window
            SetDefaultEnum<WindowMode>("Window", FrameworkSetting::WindowMode, WindowConstants::DefaultWindowMode);
            SetDefault("Window", FrameworkSetting::WindowedWidth, WindowConstants::DefaultWindowedSize.Width);
            SetDefault("Window", FrameworkSetting::WindowedHeight, WindowConstants::DefaultWindowedSize.Height);
            SetDefault("Window", FrameworkSetting::WindowedX, WindowConstants::DefaultWindowedPosition.x);
            SetDefault("Window", FrameworkSetting::WindowedY, WindowConstants::DefaultWindowedPosition.y);
            SetDefault("Window", FrameworkSetting::FullscreenMonitor, -1);
            SetDefault("Window", FrameworkSetting::FullscreenWidth, -1);
            SetDefault("Window", FrameworkSetting::FullscreenHeight, -1);
            SetDefault("Window", FrameworkSetting::FullscreenRefreshRate, -1);

            // Renderer
            SetDefaultEnum<GraphicsAPI>("Renderer", FrameworkSetting::GraphicsAPI, RendererConstants::DefaultGraphicsAPI);
            SetDefault("Renderer", FrameworkSetting::VerticalSync, RendererConstants::DefaultVerticalSync);
            SetDefault("Renderer", FrameworkSetting::AllowTearing, RendererConstants::DefaultAllowTearing);
            SetDefault("Renderer", FrameworkSetting::TripleBuffering, RendererConstants::DefaultTripleBuffering);
            SetDefaultEnum<FramerateMode>("Renderer", FrameworkSetting::FramerateMode, RendererConstants::DefaultFramerateMode);
            SetDefault("Renderer", FrameworkSetting::CustomFramerateLimit, RendererConstants::DefaultCustomFramerateLimit);
            SetDefault("Renderer", FrameworkSetting::UnfocusedFramerateLimit, RendererConstants::DefaultUnfocusedFramerateLimit);

#ifdef PXL_ENABLE_DEBUG_OVERLAY
            SetDefault("Debug", FrameworkSetting::ShowDebugOverlay, false);
#endif

            Load();
        }
    };
}