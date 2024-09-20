#pragma once
//Collision System
#include <vector>

class Component {};

class TransformComponent: public Component
{
public: 
	float m_x, m_y; //position 
	float m_width, m_height; //size
	float m_rotation;

	//constructors for transfrom 
	TransformComponent(float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f, float rotation = 0.0f)
		:m_x(x), m_y(y), m_width(width), m_height(height), m_rotation(rotation) {}

};

class ColliderComponent: public Component
{
	float width, height;
	bool isObject; //if there is an object
};
