#include"Triangle.h"
#include <algorithm>
#include <array>
#include <stdexcept>

Triangle::Triangle()
{
	v[0] << 0, 0, 0,1;
	v[1] << 0, 0, 0,1;
	v[2] << 0, 0, 0,1;

	color[0] << 0.0, 0.0, 0.0;
	color[1] << 0.0, 0.0, 0.0;
	color[2] << 0.0, 0.0, 0.0;

	texCoords[0] << 0.0, 0.0;
	texCoords[1] << 0.0, 0.0;
	texCoords[2] << 0.0, 0.0;

}



void Triangle::SetColor(int ind, Eigen::Vector3f vecColor)
{
	float r = vecColor.x();
	float g = vecColor.y();
	float b = vecColor.z();

	if ((r < 0.0) || (r > 255.) || (g < 0.0) || (g > 255.) || (b < 0.0) || (b > 255.))
	{
		throw std::runtime_error("Invalid color values");
		
		fflush(stderr);
		exit(-1);
	}
	color[ind] = vecColor;


}

std::array<Eigen::Vector4f, 3>Triangle::ToVector4() const
{
	std::array<Eigen::Vector4f, 3>res;
	std::transform(std::begin(v), std::end(v), res.begin(),
		[](auto& vec) {return Eigen::Vector4f(vec.x(), vec.y(), vec.z(), 1.f); });
	return res;
}