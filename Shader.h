#ifndef RASTERIZER_SHADER_H
#define RASTERIZER_SHADER_H
#include <Eigen/Eigen>
#include"Texture.h"
struct FragmentShaderPayload
{
	FragmentShaderPayload()
	{
		texture = nullptr;
	}
	FragmentShaderPayload(const Eigen::Vector3f& color, const Eigen::Vector3f& nor, const Eigen::Vector2f tc, Texture* tex) :
		color(color), normal(nor), texcoords(tc), texture(tex) {}
	Eigen::Vector3f viewPos;
	Eigen::Vector3f color;
	Eigen::Vector3f normal;
	Eigen::Vector2f texcoords;
	Texture* texture;
};

struct VertexShaderPayload
{
	Eigen::Vector3f position;
};

#endif//RASTERIZER_SHADER_H