#pragma once

#include "../Shader.h"

#include <vulkan/vulkan.h>

namespace pxl
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(const std::shared_ptr<GraphicsContext>& context, const std::vector<char>& vertBin, const std::vector<char>& fragBin);
        ~VulkanShader();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void Reload() override;

        virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;
        virtual void SetUniformInt1(const std::string& name, int value) override;

        VkShaderModule GetShaderModule(VkShaderStageFlagBits stage) { return m_ShaderModules[stage]; } 
    private:
        static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);
    private:
        VkDevice m_Device = VK_NULL_HANDLE;
        std::unordered_map<VkShaderStageFlagBits, VkShaderModule> m_ShaderModules;
    };
}