#pragma once

#include "ConfigParser.h"
#include <string>

class ModelLoader final
{
private:
    static std::vector<OBJModel> parseOBJ(const std::string& filePath, bool colourFill=false);
    static DirectX::XMFLOAT4 generateRandomColor(const DirectX::XMFLOAT3& minRGB = { 15, 55, 25 }, const DirectX::XMFLOAT3& maxRGB = { 65, 105, 75 });
public:
    static OBJModel loadPlayerModel();
    static OBJModel loadTreeModel();
	static std::vector<OBJModel> loadBuildingModel();
	static void randomizeModelColors(OBJModel& model);
};