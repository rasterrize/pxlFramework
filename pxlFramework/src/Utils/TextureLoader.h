#include "../Renderer/Texture.h"

namespace pxl
{
    class TextureLoader
    {
    public:
        static std::shared_ptr<Texture> Load(const std::string& filePath);
    };
}