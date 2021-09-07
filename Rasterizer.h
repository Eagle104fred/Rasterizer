#pragma once
#include <algorithm>
#include<Eigen/Eigen>
#include"Triangle.h"
#include"Shader.h"
#include<string>
#include "nonstd/optional.hpp"
using namespace Eigen;

namespace Rst{
	//KS: 基本功能
	
	//KS: 这是预设的颜色 
	static struct Color{
		Eigen::Vector3f green = Eigen::Vector3f(0, 255, 0);
		Eigen::Vector3f red = Eigen::Vector3f(0, 0, 255);
		Eigen::Vector3f blue = Eigen::Vector3f(255, 0,0 );
		Eigen::Vector3f white = Eigen::Vector3f(0, 0, 0);
	}Color;
	
	enum class Primitive
	{
		Line,
		Fill
	};
	//KS: Shader类型 
	enum class ShaderType
	{
		Normal,
		Phong,
		Bump,
		Displacement,
		Texture
	};

	// 	定义缓冲区
	enum class Buffer
	{
		Depth = 1,
		Color = 2
	};
	inline Buffer operator|(Buffer a, Buffer b)
	{
		return Buffer((int)a | (int)b);
	}

	inline Buffer operator&(Buffer a, Buffer b)
	{
		return Buffer((int)a & (int)b);
	}

	//统计map的个数
	struct PosId
	{
		int id = 0;
	};
	struct IndId
	{
		int id = 0;
	};
	struct ColId
	{
		int id = 0;
	};
	class Rasterizer
	{
	public:
		Rasterizer(int w, int h);
		void SetModel(float rotationX, float rotationY, float rotationZ);
		void SetView(Eigen::Vector3f eyePos);
		void SetProjection(float camFov,float aspectRatio,float zNear,float zFar);
		void SetPixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color);
		void SetPixel(const Eigen::Vector2i& point, const Eigen::Vector3f& color);
		void DrawTriangle(PosId Positionid, IndId indexId,ColId colId, Primitive type);
		void DrawModel(std::vector<Triangle *>&TriangleList);

		void SetVertexShader(std::function<Eigen::Vector3f(VertexShaderPayload)> VertShader);
		void SetFragmentShader(std::function<Eigen::Vector3f(FragmentShaderPayload)> FargShader);
		
		PosId LoadPosition(std::vector<Eigen::Vector3f>& pos);
		IndId LoadIndex(std::vector<Eigen::Vector3i>& ind);
		ColId LoadColor(std::vector<Eigen::Vector3f>& col);


		std::vector<Eigen::Vector3f>& GetFrameBuffer() { return frameBuffer; };

		void Clear(Buffer buff);

	public:
		//KS: shader
		void SelectShader(ShaderType type);
		
	private:
		void DrawLine(Eigen::Vector4f begin, Eigen::Vector4f end, Eigen::Vector3f color);
		void RasterizeTriangleLine(const Triangle& t);
		void RasterizeTriangleFill(const Triangle& t);
		void RasterizerModel(const Triangle& t, const std::array<Eigen::Vector3f, 3>& view_pos);
		bool InsideTriangle(float x, float y, const Eigen::Vector4f* tri);
		std::tuple<float, float, float> ComputeBarycentric2D(float x, float y, const Vector4f* tri);



		
	private:
		int GetNextId() { return next_id++; }
		int GetIndex(int x, int y) { return (height - 1-y) * width + x; }
		Eigen::Vector4f ToVec4(const Eigen::Vector3f& v3, float w = 1.0f)
		{
			return Eigen::Vector4f(v3.x(), v3.y(), v3.z(), w);
		}

		Eigen::Matrix4f model;
		Eigen::Matrix4f view;
		Eigen::Matrix4f projection;
		std::vector<Eigen::Vector3f> frameBuffer;

		std::vector<float>depthBuffer;

		std::map<int, std::vector<Eigen::Vector3f>> posMap;
		std::map<int, std::vector<Eigen::Vector3i>>indMap;
		std::map<int, std::vector<Eigen::Vector3f>>colMap;

		int width;
		int height;

		int next_id = 0;

		//KS: 显示obj模型 
		nonstd::optional<Texture> texture;//KS: nonstd 用于在cpp14实现optional 
		std::function<Eigen::Vector3f(VertexShaderPayload)> vertShaderFunc;
		std::function<Eigen::Vector3f(FragmentShaderPayload)>fragmentShaderFunc;

		
	};

}
