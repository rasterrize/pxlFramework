#include "../Renderer/Texture.h"

namespace pxl
{
    class TextureLoader
    {
    public:
        static Texture* Load(const std::string& filePath);
    };
}