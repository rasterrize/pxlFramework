#include "TextureSheet.h"

#include "Renderer/Primitives/2D/Quad.h"

namespace pxl
{
    TextureSheet::TextureSheet(std::shared_ptr<Texture> texture, uint32_t columns, uint32_t rows)
        : m_Texture(texture), m_Columns(columns), m_Rows(rows)
    {
        m_SubTextures.reserve(columns * rows);
        glm::vec2 gridSize = glm::vec2(m_Columns, m_Rows);
        auto percentagePerRegion = glm::vec2(1) / gridSize;

        for (int row = 1; row <= m_Rows; row++)
        {
            for (int column = 1; column <= m_Columns; column++)
            {
                glm::vec2 gridPos = glm::vec2(column, row) - glm::vec2(1);

                // Quad default tex coords are the same in this case, so we'll just use those
                auto coords = Quad::GetDefaultTexCoords();
                auto bottomLeftPos = gridPos / gridSize;

                // Precalculate each texture coordinate for the subtexture
                for (auto& coord : coords)
                {
                    coord = bottomLeftPos + (coord * percentagePerRegion);
                }

                m_SubTextures.emplace_back(texture, coords);
            }
        }
    }

    const SubTexture& TextureSheet::GetSubTexture(uint32_t x, uint32_t y) const
    {
        PXL_ASSERT(x <= m_Columns && y <= m_Rows);
        return m_SubTextures.at((y - 1) * m_Columns + x);
    }

    std::vector<SubTexture> TextureSheet::GetSubTexturesRange(const glm::uvec2& startPos, const glm::uvec2& endPos)
    {
        // TODO: check start pos is less than end pos
        // TODO: check start and end pos are in bounds of grid

        auto startValue = (startPos.y - 1) * m_Columns + (startPos.x - 1);
        auto endValue = (endPos.y - 1) * m_Columns + endPos.x;
        return std::vector<SubTexture>(m_SubTextures.begin() + startValue, m_SubTextures.begin() + endValue);
    }
}
