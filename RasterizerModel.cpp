#include <algorithm>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <stdexcept>
#include"Rasterizer.h"
#include"Triangle.h"
#include "Global.h"

void Rst::Rasterizer::SetVertexShader(std::function<Eigen::Vector3f(VertexShaderPayload) > VertShader)
{
	vertShaderFunc = VertShader;
}

void Rst::Rasterizer::SetFragmentShader(std::function<Eigen::Vector3f(FragmentShaderPayload)>FragShader)
{
	fragmentShaderFunc = FragShader;
}

//KS: 差值函数 
static Eigen::Vector3f Interpolate(float alpha, float beta, float gamma,
	const Eigen::Vector3f& ver1, const Eigen::Vector3f& vert2, const Eigen::Vector3f& vert3, float weight)
{
	return (alpha * ver1 + beta * vert2 + gamma * vert3) / weight;
}
static Eigen::Vector2f Interpolate(float alpha, float beta, float gamma,
	const Eigen::Vector2f& vert1, const Eigen::Vector2f& vert2, const Eigen::Vector2f& vert3, float weight)
{
	auto u = (alpha * vert1[0] + beta * vert2[0] + gamma * vert3[0]);
	auto v = (alpha * vert1[1] + beta * vert2[1] + gamma * vert3[1]);

	u /= weight;
	v /= weight;
	return Eigen::Vector2f(u, v);
}

void Rst::Rasterizer::RasterizeModel(const Triangle& t, const std::array<Eigen::Vector3f, 3>& viewPos)
{
	auto v = t.ToVector4();

	float minX = std::floor(std::min(v[0][0], std::min(v[1][0], v[2][0])));
	float minY = std::floor(std::min(v[0][1], std::min(v[1][1], v[2][1])));
	float maxX = std::ceil(std::max(v[0][0], std::max(v[1][0], v[2][0])));
	float maxY = std::ceil(std::max(v[0][1], std::max(v[1][1], v[2][1])));

	for (int x = minX; x <= maxX; x++)
	{
		for (int y = minY; y <= maxY; y++)
		{
			if (InsideTriangle(x, y, t.v))
			{
				//KS: 计算重心坐标
				auto abg = ComputeBarycentric2D((float)x + 0.5f, (float)y + 0.5f, t.v);
				auto alpha = std::get<0>(abg);
				auto beta = std::get<1>(abg);
				auto gamma = std::get<2>(abg);
				float w = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
				float z = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
				z *= w;
				if (depthBuffer[GetIndex(x, y)] > z)
				{
					auto interpolatedColor = Interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], 1);
					auto interpolatedNormal = Interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], 1);
					auto interpolatedTexcoords = Interpolate(alpha, beta, gamma, t.texCoords[0], t.texCoords[1], t.texCoords[2], 1);
					auto interpolatedShadingcoords = Interpolate(alpha, beta, gamma, viewPos[0], viewPos[1], viewPos[2], 1);
					depthBuffer[GetIndex(x, y)] = z;

					FragmentShaderPayload payload(interpolatedColor, interpolatedNormal.normalized() , interpolatedTexcoords, texture ? &*texture : nullptr);
					payload.viewPos = interpolatedShadingcoords;
					auto pixelColor = fragmentShaderFunc(payload);
					SetPixel(Eigen::Vector2i(x, y), pixelColor);
				}
			}
		}
	}

}

void Rst::Rasterizer::DrawModel(std::vector<Triangle*>& TriangleList)
{
	float f1 = (50 - 0.1) / 2.0;
	float f2 = (50 + 0.1) / 2.0;
	Eigen::Matrix4f mvp = projection * view * model;
	int count = 0;
	for (const auto& t : TriangleList)
	{
		Triangle newTri =*t;

		//对三角形的坐标变换处理为了后续再三角形内插值使用。
		//并且后续shader中的光的方向向量、视线的方向向量等都是需要基于真实坐标计算
		std::array<Eigen::Vector4f, 3>mm{
			(view * model * t->v[0]),
			(view * model * t->v[1]),
			(view * model * t->v[2])
		};
		std::array<Eigen::Vector3f, 3>viewSpacePos;
		std::transform(mm.begin(), mm.end(), viewSpacePos.begin(), [](auto& v) {
			return v.template head<3>();
			});

		Eigen::Vector4f v[] = {
			mvp * t->v[0],
			mvp *  t->v[1],
			mvp * t->v[2]
		};

		//KS: 齐次除法 
		for (auto& vec : v)
		{
			vec.x() /= vec.w();
			vec.y() /= vec.w();
			vec.z() /= vec.w();
		}

		Eigen::Matrix4f inv_trans = (view * model).inverse().transpose();
		Eigen::Vector4f n[] = {
			inv_trans * ToVec4(t->normal[0], 0.0f),
			inv_trans * ToVec4(t->normal[1], 0.0f),
			inv_trans * ToVec4(t->normal[2], 0.0f)
		};

		//Viewport transformation窗口到视口的变幻, 缩放因子为0.5
		for (auto& vert : v)
		{
			vert.x() = 0.5 * width * (vert.x() + 1.0);
			vert.y() = 0.5 * height * (vert.y() + 1.0);
			vert.z() = vert.z() * f1 + f2;
		}
		for (int i = 0; i < 3; i++)
		{
			newTri.SetVertex(i, v[i]);
		}

		for (int i = 0; i < 3; i++)
		{
			newTri.SetNormal(i, n[i].head<3>());
		}
		newTri.SetColor(0, Eigen::Vector3f(148, 121.0, 92.0));
		newTri.SetColor(1, Eigen::Vector3f(148, 121.0, 92.0));
		newTri.SetColor(2, Eigen::Vector3f(148, 121.0, 92.0));

		RasterizeModel(newTri, viewSpacePos);

	}
}
//KS: Shader 
Eigen::Vector3f NormalFragmentShader(const FragmentShaderPayload& payload)
{
	Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0f, 1.0f, 1.0f)) / 2.f;
	Eigen::Vector3f result;
	result << return_color.x() * 255, return_color.y() * 255, return_color.z() * 255;
	return result;
}

//KS: 选择Shader 
void Rst::Rasterizer::SelectShader(ShaderType type)
{
	switch (type)
	{
	case Rst::ShaderType::Normal:
		fragmentShaderFunc = NormalFragmentShader;
		break;
	case Rst::ShaderType::Phong:
		break;
	case Rst::ShaderType::Bump:
		break;
	case Rst::ShaderType::Displacement:
		break;
	case Rst::ShaderType::Texture:
		break;
	default:
		break;
	}
}


