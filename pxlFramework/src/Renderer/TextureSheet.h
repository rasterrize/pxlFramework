#pragma once

#include <glm/vec2.hpp>

#include "Renderer/Texture.h"

namespace pxl
{
    class TextureSheet
    {
    public:
        TextureSheet(std::shared_ptr<Texture> texture, uint32_t columns, uint32_t rows);

        const SubTexture& GetSubTexture(uint32_t x, uint32_t y) const;
        const std::vector<SubTexture>& GetSubTextures() const { return m_SubTextures; }
        std::vector<SubTexture> GetSubTexturesRange(const glm::uvec2& startPos, const glm::uvec2& endPos);

    private:
        std::shared_ptr<Texture> m_Texture = nullptr;
        uint32_t m_Columns;
        uint32_t m_Rows;

        std::vector<SubTexture> m_SubTextures;
    };
}
