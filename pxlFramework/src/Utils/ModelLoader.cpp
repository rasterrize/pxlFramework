#include "ModelLoader.h"

//#include <iostream>
#include <fstream>

namespace pxl
{
    std::vector<Mesh> ModelLoader::s_Meshes;

    bool ModelLoader::LoadOBJ(const std::string& filePath)
    {
        std::ifstream file(filePath);

        std::string line;
        std::getline(file, line);

        Logger::LogInfo(line);
        return false;
    }
}