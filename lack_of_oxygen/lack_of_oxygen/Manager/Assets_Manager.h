

#pragma once
/**************Header files*****************/

#ifndef LOF_ASSETS_MANAGER_H
#define LOF_ASSETS_MANAGER_H




//#define GLFW_INCLUDE_NONE
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <map>

#include "../Manager/Manager.h"

namespace lof 
{
	template <typename T>
	class Assets_Manager: public manager
	{
		//std::shared_ptr<T> assets;
		std::map<std::string, std::shared_ptr<T>> assets;

	public:
		//std::shared_ptr<T> Get_Texture(const std::string& name);
		Load_Texture(std::string name, std::string file_Name);

		Get_Texture(std::string name);

		Load_Font(std::string name, std::string file_Name);

		Get_Font(std::string name);
	};

} //end of namespace lof


#endif // !LOF _ASSETS_MANAGER_H