#include <algorithm>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <stdexcept>
#include"Rasterizer.h"
#include"Triangle.h"
#include"Global.h"



//KS:  mvp��������
void Rst::Rasterizer::SetModel(float rotationX, float rotationY, float rotationZ)//KS:  ��ת����
{

    float angleZ = rotationZ * MY_PI / 180;
    float angleY = rotationY * MY_PI / 180;
    float angleX = rotationX * MY_PI / 180;
    Eigen::Matrix4f tempZ = Eigen::Matrix4f::Identity();
    tempZ <<
        std::cos(angleZ), -std::sin(angleZ), 0, 0,
        std::sin(angleZ), std::cos(angleZ), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;
    Eigen::Matrix4f tempY = Eigen::Matrix4f::Identity();

    tempY <<
        std::cos(angleY), 0, std::sin(angleY), 0,
        0, 1, 0, 0,
        -std::sin(angleY), 0, std::cos(angleY), 0,
        0, 0, 0, 1;
    Eigen::Matrix4f tempX = Eigen::Matrix4f::Identity();

    tempX <<
        1, 0, 0, 0,
        0, std::cos(angleX), -std::sin(angleX), 0,
        0, std::sin(angleX), std::cos(angleX), 0,
        0, 0, 0, 1;
    model = tempX * tempY * tempZ;

}
void Rst::Rasterizer::SetView(Eigen::Vector3f eyePos)
{
    //KS: ���۲�������������任����任 
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f translate;
    translate <<
        1, 0, 0, -eyePos[0],
        0, 1, 0, -eyePos[1],
        0, 0, 1, -eyePos[2],
        0, 0, 0, 1;
    view = translate * view;//KS: set view matrix 

}
void Rst::Rasterizer::SetProjection(float eyeFov, float aspectRatio, float zNear, float zFar)
{
    Eigen::Vector4f temp = Eigen::Vector4f::Identity();
    auto n = zNear;
    auto f = zFar;

    //KS: https://smuwm007.feishu.cn/docs/doccn2FNKtTm58i2R4jISC7vd4e#KufB6G 
    Eigen::Matrix4f p2o;
    p2o <<
        n, 0, 0, 0,
        0, n, 0, 0,
        0, 0, f + n, -f * n,
        0, 0, 1, 0;

    float halve = eyeFov / 2 * MY_PI / 180;//KS: �ǵ�ת���� 
    float top = -zNear * std::tan(halve);
    float bottom = -top;

    float right = top * aspectRatio;
    float left = -right;

    Eigen::Matrix4f m, s;
    m <<
        1, 0, 0, -(left + right) / 2,
        0, 1, 0, -(top + bottom) / 2,
        0, 0, 1, -(zNear + zFar) / 2,
        0, 0, 0, 1;

    s <<
        2 / (right - left), 0, 0, 0,
        0, 2 / (top - bottom), 0, 0,
        0, 0, 2 / (zNear - zFar), 0,
        0, 0, 0, 1;
     
    projection = p2o * m * s;

}


//////////////////////////////////////////////////////////////
// ��ܻ�������
//////////////////////////////////////////////////////////////
//KS: ��ʼ������ 
Rst::Rasterizer::Rasterizer(int w, int h) :width(w), height(h)
{
    frameBuffer.resize(w * h);
    depthBuffer.resize(w * h);

    texture = nonstd::nullopt;//KS: nonstd ������cpp14ʵ��optional 
}
//KS:���������ֵ� 
Rst::PosId Rst::Rasterizer::LoadPosition(std::vector<Eigen::Vector3f>& pos)
{
    auto id = Rst::Rasterizer::GetNextId();
    posMap.emplace(id, pos);
    return { id };

}
//KS: ���������ֵ� 
Rst::IndId Rst::Rasterizer::LoadIndex(std::vector<Eigen::Vector3i>& ind)
{
    auto id = Rst::Rasterizer::GetNextId();
    indMap.emplace(id, ind);
    return { id };
}
//KS: ������ɫ�ֵ� 
Rst::ColId Rst::Rasterizer::LoadColor(std::vector<Eigen::Vector3f>& col)
{
    auto id = Rst::Rasterizer::GetNextId();
    colMap.emplace(id, col);
    return { id };
}
//KS: �����ɫ�������Ȼ��� 
void Rst::Rasterizer::Clear(Buffer buff)
{
    if ((buff & Rst::Buffer::Color) == Rst::Buffer::Color)
    {
        std::fill(frameBuffer.begin(), frameBuffer.end(), Eigen::Vector3f{ 0,0,0 });
    }
    if ((buff & Rst::Buffer::Depth) == Rst::Buffer::Depth)
    {
        std::fill(depthBuffer.begin(), depthBuffer.end(), std::numeric_limits<float>::infinity());
    }
}

//KS: ��ɫ 
void Rst::Rasterizer::SetPixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //KS: Խ���ж� 
    if (point.x() < 0 || point.x() >= width
        || point.y() < 0 || point.y() >= height)return;

    auto ind = (height - 1 - point.y()) * width + point.x();

    frameBuffer[ind] = color;
}
void Rst::Rasterizer::SetPixel(const Eigen::Vector2i& point, const Eigen::Vector3f& color)
{
    int ind = (height  - point.y()) * width + point.x();

    frameBuffer[ind] = color;
}


//KS: ���Ƿ�����������
bool Rst::Rasterizer::InsideTriangle(float x, float y, const Eigen::Vector4f* tri)
{
    Eigen::Vector2f point(x, y);
    Eigen::Vector2f AB = (tri[1].head(2) - tri[0].head(2));
    Eigen::Vector2f BC = (tri[2].head(2) - tri[1].head(2));
    Eigen::Vector2f CA = (tri[0].head(2) - tri[2].head(2));

    Eigen::Vector2f AP = (point - tri[0].head(2));
    Eigen::Vector2f BP = (point - tri[1].head(2));
    Eigen::Vector2f CP = (point - tri[2].head(2));

    return  AB[0] * AP[1] - AB[1] * AP[0] > 0 &&
        BC[0] * BP[1] - BC[1] * BP[0] > 0 &&
        CA[0] * CP[1] - CA[1] * CP[0] > 0;

}
//KS: ����������� 
std::tuple<float, float, float> Rst::Rasterizer::ComputeBarycentric2D(float x, float y, const Vector4f* tri)
{
    //KS: https://blog.csdn.net/why18767183086/article/details/107369094 
    float xp = x, yp = y;
    float xa = tri[0].x(), ya = tri[0].y();
    float xb = tri[1].x(), yb = tri[1].y();
    float xc = tri[2].x(), yc = tri[2].y();
    float gamma = (((xb - xa) * (yp - ya) - (xp - xa) * (yb - ya)) / (((xb - xa) * (yc - ya)) - (yb - ya) * (xc - xa)));
    float beta = (xp - xa - gamma * (xc - xa)) / (xb - xa);
    float alpha = 1.0f - beta - gamma;
    /*float alpha = (x * (tri[1].y() - tri[2].y()) + (tri[2].x() - tri[1].x()) * y + tri[1].x() * tri[2].y() - tri[2].x() * tri[1].y()) / (tri[0].x() * (tri[1].y() - tri[2].y()) + (tri[2].x() - tri[1].x()) * tri[0].y() + tri[1].x() * tri[2].y() - tri[2].x() * tri[1].y());
    float beta = (x * (tri[2].y() - tri[0].y()) + (tri[0].x() - tri[2].x()) * y + tri[2].x() * tri[0].y() - tri[0].x() * tri[2].y()) / (tri[1].x() * (tri[2].y() - tri[0].y()) + (tri[0].x() - tri[2].x()) * tri[1].y() + tri[2].x() * tri[0].y() - tri[0].x() * tri[2].y());
    float gamma = (x * (tri[0].y() - tri[1].y()) + (tri[1].x() - tri[0].x()) * y + tri[0].x() * tri[1].y() - tri[1].x() * tri[0].y()) / (tri[2].x() * (tri[0].y() - tri[1].y()) + (tri[1].x() - tri[0].x()) * tri[2].y() + tri[0].x() * tri[1].y() - tri[1].x() * tri[0].y());*/
    return { alpha,beta,gamma };
}