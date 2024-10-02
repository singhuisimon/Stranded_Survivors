

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
#include <map> //std::map

#include "../Manager/Manager.h"

namespace lof 
{
	template <typename T>
	class Assets_Manager: public manager
	{
		//std::shared_ptr<T> assets; map to  store assets with their names as keys
		std::map<std::string, std::shared_ptr<T>> assets;

	public:
		//std::shared_ptr<T> Get_Texture(const std::string& name);
		void Load_Texture(std::string name, std::string file_Name);

		std::shared_ptr<T>Get_Texture(std::string name);

		void Load_Font(std::string name, std::string file_Name);

		std::shared_ptr<T>Get_Font(std::string name);

		bool Assets_Exists(const std::string& name) const;

		void Unload_Assets(const std::string name);

	};

} //end of namespace lof



#endif // !LOF _ASSETS_MANAGER_H