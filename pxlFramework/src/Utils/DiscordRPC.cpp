#include "DiscordRPC.h"

namespace pxl
{
    bool DiscordRPC::s_Enabled = false;
    std::unique_ptr<discord::Core> DiscordRPC::s_DiscordAPI;

    static void LogCallback(discord::LogLevel minLevel, const char* message)
    {
        PXL_LOG_ERROR(LogArea::Other, "Discord Error: {}", message);
    }

    static void ActivityCallback(discord::Result result)
    {
    #ifdef PXL_ENABLE_LOGGING
        if (result == discord::Result::Ok)
            PXL_LOG_INFO(LogArea::Other, "Discord activity updated")
        else
            PXL_LOG_WARN(LogArea::Other, "Discord activity failed to update");
    #endif
    }

    void DiscordRPC::Init(int64_t clientID)
    {
        if (s_Enabled)
            return;

        discord::Core* api = nullptr;
        auto result = discord::Core::Create(clientID, DiscordCreateFlags_NoRequireDiscord, &api); // discord claims that the memory is freed automatically on its own, I should confirm this later
        s_DiscordAPI.reset(api);

        if (result != discord::Result::Ok)
        {
            PXL_LOG_INFO(LogArea::Other, "Failed to initialize Discord RPC");
            return;
        }

        if (api)
        {
            PXL_LOG_INFO(LogArea::Other, "Discord RPC instance successfully created");
        }
        else
        {
            PXL_LOG_WARN(LogArea::Other, "Failed to create DiscordRPC core instance");
            return;
        }

        s_DiscordAPI->SetLogHook(discord::LogLevel::Error, LogCallback);

        s_Enabled = true;
    }

    void DiscordRPC::Update()
    {
        if (!s_Enabled)
            return;

        auto result = s_DiscordAPI->RunCallbacks();

        if (result == discord::Result::NotRunning)
        {
            PXL_LOG_WARN(LogArea::Other, "Detected discord not running, closing discord rpc");
            s_Enabled = false;
            return;
        }
    }

    void DiscordRPC::SetPresence(const DiscordRPCSettings& settings)
    {
        if (!s_Enabled)
            return;
        
        discord::Activity activity = {};
        activity.SetApplicationId(settings.ClientID);
        activity.SetName(settings.AppName.c_str());
        activity.SetDetails(settings.Details.c_str());
        activity.GetAssets().SetLargeImage(settings.LargeImageKey.c_str());
        discord::ActivityType activityType = discord::ActivityType::Playing;
        switch (settings.ActivityType)
        {
            case DiscordRPCActivityType::Playing:
                activityType = discord::ActivityType::Playing;
                break;
            case DiscordRPCActivityType::Streaming:
                activityType = discord::ActivityType::Streaming;
                break;
            case DiscordRPCActivityType::Listening:
                activityType = discord::ActivityType::Listening;
                break;
            case DiscordRPCActivityType::Watching:
                activityType = discord::ActivityType::Watching;
                break;
        }
        activity.SetType(activityType);

        s_DiscordAPI->ActivityManager().UpdateActivity(activity, ActivityCallback);
    }
}