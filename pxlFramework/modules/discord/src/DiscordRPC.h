#include <discord.h>

namespace pxl
{
    enum class DiscordRPCActivityType
    {
        Playing,
        Streaming,
        Listening,
        Watching,
    };

    struct DiscordRPCSettings
    {
        const int64_t ClientID;
        std::string AppName; // likely won't work. Uses discord developer application name
        std::string Details;
        DiscordRPCActivityType ActivityType;
        std::string LargeImageKey;
    };

    class DiscordRPC
    {
    public:
        static void Init(int64_t clientID);
        static void Shutdown() { s_Enabled = false; }
        static void Update();
        static void SetPresence(const DiscordRPCSettings& settings);

        static const DiscordRPCSettings& GetCurrentSettings() { return s_Settings; }

    private:
        static bool s_Enabled;

        static std::unique_ptr<discord::Core> s_DiscordAPI;

        static DiscordRPCSettings s_Settings;
    };
}