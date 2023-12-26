namespace pxl
{
    class Device
    {
    public:
        virtual ~Device() = default;

        virtual void* GetNativeDevice() = 0;
        virtual void Destroy() = 0;
    };
}