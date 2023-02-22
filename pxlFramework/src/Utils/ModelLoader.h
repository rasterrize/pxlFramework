#include <glm/glm.hpp>

namespace pxl
{
    struct Mesh
    {
        std::vector<glm::vec3> m_Vertices;
        std::vector<unsigned int> m_Indices;
    };

    class ModelLoader
    {
    public:
        static bool LoadOBJ(const std::string& filePath);
        static std::vector<Mesh> GetMeshes() { return s_Meshes; }
    private:
        static std::vector<Mesh> s_Meshes;
    };
}