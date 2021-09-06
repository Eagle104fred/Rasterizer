#ifndef RASTERIZER_TRIANGLE_H
#define RASTERIZER_TRIANGLE_H
#include<Eigen/Eigen>

using namespace Eigen;
class Triangle
{
public:
	Vector4f v[3];//KS: �����ε���������//KS: Ϊ�˺�MVP�������Կ����Vector3f��ΪVector4f
	
	Vector3f color[3];//KS: �����������ɫ
	Vector3f normal[3];//KS: ��������ķ�������;
	Vector2f texCoords[3];
	Triangle() ;

	Vector4f a() const { return v[0]; }
	Vector4f b() const { return v[1]; }
	Vector4f c() const { return v[2]; }

	void SetVertex(int ind, Eigen::Vector4f ver) { v[ind] = ver; }
	void SetNormal(int ind, Vector3f n) { normal[ind] = n; }
	void SetColor(int ind, Vector3f color);
	void SetTexCoord(int ind, Vector2f uv) { texCoords[ind] = uv; }
	Vector3f GetColor() const{ return color[0]; }
	std::array<Vector4f, 3>ToVector4()const;

};



#endif // RASTERIZER_TRIANGLE_H
