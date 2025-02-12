#include "OpenGLTexture.h"

namespace pxl
{
    OpenGLTexture::OpenGLTexture(const Image& image, const TextureSpecs& specs)
        : m_Metadata(image.Metadata), m_Specs(specs)
    {
        CreateTexture(image.Buffer);
    }

    OpenGLTexture::OpenGLTexture(const std::shared_ptr<Image>& image, const TextureSpecs& specs)
        : m_Metadata(image->Metadata), m_Specs(specs)
    {
        CreateTexture(image->Buffer);
    }

    void OpenGLTexture::CreateTexture(const std::vector<uint8_t>& pixels)
    {
        const int32_t lod = 0;
        const int32_t border = 0; // docs.gl states this MUST be 0
        int32_t width = static_cast<int32_t>(m_Metadata.Size.Width);
        int32_t height = static_cast<int32_t>(m_Metadata.Size.Height);

        GLenum textureType = ToGLType(m_Specs.Type);
        GLenum imageFormat = ToGLFormat(m_Metadata.Format);
        GLenum wrapMode = ToGLWrapMode(m_Specs.WrapMode);
        GLenum sampleFilter = ToGLFilter(m_Specs.Filter);

        glCreateTextures(textureType, 1, &m_RendererID);
        glBindTexture(textureType, m_RendererID);

        // Set sampling filter
        glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, sampleFilter);
        glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, sampleFilter);

        // Set wrap mode
        glTexParameteri(textureType, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(textureType, GL_TEXTURE_WRAP_T, wrapMode);

        switch (textureType)
        {
            case GL_TEXTURE_2D:
                glTexImage2D(textureType, lod, imageFormat, width, height, border, imageFormat, GL_UNSIGNED_BYTE, pixels.data());
                break;

            default:
                PXL_LOG_ERROR(LogArea::OpenGL, "Invalid texture type");
        }

        Unbind();
    }

    OpenGLTexture::~OpenGLTexture()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture::SetData(const void* data)
    {
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Metadata.Size.Width, m_Metadata.Size.Height, ToGLFormat(m_Metadata.Format), GL_UNSIGNED_BYTE, data);
    }

    void OpenGLTexture::Bind(uint32_t unit)
    {
        glBindTextureUnit(unit, m_RendererID);
    }

    void OpenGLTexture::Unbind()
    {
        glBindTexture(ToGLType(m_Specs.Type), 0);
    }

    GLenum OpenGLTexture::ToGLFormat(ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::Undefined: return GL_INVALID_ENUM;
            case ImageFormat::RGB8:      return GL_RGB;
            case ImageFormat::RGBA8:     return GL_RGBA;
        }

        return GL_INVALID_ENUM;
    }

    GLenum OpenGLTexture::ToGLFilter(SampleFilter filter)
    {
        switch (filter)
        {
            case SampleFilter::Undefined: return GL_INVALID_ENUM;
            case SampleFilter::Nearest:   return GL_NEAREST;
            case SampleFilter::Linear:    return GL_LINEAR;
        }

        return GL_INVALID_ENUM;
    }

    GLenum OpenGLTexture::ToGLType(TextureType type)
    {
        switch (type)
        {
            case TextureType::Tex1D:   return GL_TEXTURE_1D;
            case TextureType::Tex2D:   return GL_TEXTURE_2D;
            case TextureType::Tex3D:   return GL_TEXTURE_3D;
            case TextureType::CubeMap: return GL_TEXTURE_CUBE_MAP;
        }

        return GL_INVALID_ENUM;
    }

    GLenum OpenGLTexture::ToGLWrapMode(TextureWrap mode)
    {
        switch (mode)
        {
            case TextureWrap::Repeat:         return GL_REPEAT;
            case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
            case TextureWrap::ClampToEdge:    return GL_CLAMP_TO_EDGE;
            case TextureWrap::ClampToBorder:  return GL_CLAMP_TO_BORDER;
        }

        return GL_INVALID_ENUM;
    }
}