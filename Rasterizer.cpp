#include <algorithm>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <stdexcept>
#include"Rasterizer.h"
#include"Triangle.h"
#include "Global.h"



void Rst::Rasterizer::DrawTriangle(Rst::PosId posBuf,Rst::IndId indBuf,Rst::ColId colBuf, Primitive type)
{
    //KS: 按照id取出三角形 
    auto& buf = posMap[posBuf.id];
    auto& ind = indMap[indBuf.id];
    auto& col = colMap[colBuf.id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;

        

        //KS: 使用索引取出每个三角形顶点 
        Eigen::Vector4f v[] = {
            mvp * ToVec4(buf[i[0]],1.0f),
            mvp * ToVec4(buf[i[1]], 1.0f),
            mvp * ToVec4(buf[i[2]],1.0f)

        };
        //KS: 透视除法 Homogeneous division齐次除法是的w为1
        for (auto& vec : v)
        {
            vec /= vec.w();
        }

        //KS: 视口变换
        for (auto& vert : v)
        {
            vert.x() = 0.5 * width * (vert.x() + 1.0);
            vert.y() = 0.5 * height * (vert.y() + 1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; i++)
        {   

            t.SetVertex(i, v[i]);
            //KS: ???疑问???
            /*不太清楚为什么Games101的代码框架中这里要重复设置三次
            t.SetVertex(i, v[i].head<3>());
            t.SetVertex(i, v[i].head<3>());*/
        }
        auto colX = col[i[0]];
        auto colY = col[i[1]];
        auto colZ = col[i[2]];

        t.SetColor(0, colX);
        t.SetColor(1, colY);
        t.SetColor(2, colZ);

        if (type == Rst::Primitive::Line)
        {
            RasterizeTriangleLine(t);//KS: 线框模型 
        }
        else if (type == Rst::Primitive::Fill)
        {
            RasterizeTriangleFill(t);//KS: 重心坐标着色 
        }
        else
        {
            throw std::runtime_error("Drawing primitives other than triangle is not implemented yet!");
        }
            
    }

}


//KS: 着色模型 
void Rst::Rasterizer::RasterizeTriangleFill(const Triangle& t)
{
    auto tri = t.ToVector4();
    //KS: 确定图形的aabb大小
    float minX = std::floor(std::min(tri[0][0], std::min(tri[1][0], tri[2][0])));//KS: 拿出ABC点的x坐标
    float minY = std::floor(std::min(tri[0][1], std::min(tri[1][1], tri[2][1])));//KS: 拿出ABC点的y坐标
    float maxX = std::ceil(std::max(tri[0][0], std::max(tri[1][0], tri[2][0])));//KS: 拿出ABC点的x坐标
    float maxY = std::ceil(std::max(tri[0][1], std::max(tri[1][1], tri[2][1])));//KS: 拿出ABC点的y坐标

    //KS: 抗锯齿在global.h中设置
    if (MSAA)
    {
        std::vector<Eigen::Vector2f> pos
        {
            {0.25,0.25},
            {0.75,0.25},
            {0.25,0.75},
            {0.75,0.75},
        };
        for (int x = minX; x <= maxX; x++)
        {
            for (int y = minY; y <= maxY; y++)
            {
                int count = 0;
                for (int i = 0; i < 4; i++)
                {
                    if (InsideTriangle((float)x+pos[i][0], (float)y+pos[i][1], t.v))
                    {
                        //KS: 计算重心坐标
                        auto abg = ComputeBarycentric2D((float)x + pos[i][0], (float)y + pos[i][1], t.v);
                        auto alpha = std::get<0>(abg);
                        auto beta = std::get<1>(abg);
                        auto gamma = std::get<2>(abg);

                        float w = 1.0 / (alpha / tri[0].w() + beta / tri[1].w() + gamma / tri[2].w());//KS: 视口变换
                        float z = alpha * tri[0].z() / tri[0].w() + beta * tri[1].z() / tri[1].w() + gamma * tri[2].z() / tri[2].w();
                        z *= w;

                        count++;
                        if (count != 0)
                        {
                            //KS: 着色
                            if (depthBuffer[GetIndex(x, y)] > z)
                            {
                                //Eigen::Vector3f point = Eigen::Vector3f(x, y, z);
                                Eigen::Vector2i point = Eigen::Vector2i(x, y);
                                SetPixel(point, t.GetColor()*count/4.0);
                                depthBuffer[GetIndex(x, y)] = z;
                            }
                        }
                    }
                }
                
            }
        }
    }
    else
    {
        for (int x = minX; x <= maxX; x++)
        {
            for (int y = minY; y <= maxY; y++)
            {
                if (InsideTriangle(x, y, t.v))
                {
                    //KS: 计算重心坐标
                    auto abg = ComputeBarycentric2D(x + 0.5f, y + 0.5f, t.v);
                    auto alpha = std::get<0>(abg);
                    auto beta = std::get<1>(abg);
                    auto gamma = std::get<2>(abg);

                    float w = 1.0 / (alpha / tri[0].w() + beta / tri[1].w() + gamma / tri[2].w());//KS: 视口变换
                    float z = alpha * tri[0].z() / tri[0].w() + beta * tri[1].z() / tri[1].w() + gamma * tri[2].z() / tri[2].w();
                    z *= w;
                    //KS: 着色                                            
                    if (depthBuffer[GetIndex(x, y)] > z)
                        
                    {
                        Eigen::Vector3f point = Eigen::Vector3f(x, y, z);
                        SetPixel(point, t.GetColor());
                        depthBuffer[GetIndex(x, y)] = z;
                    }
                }
            }
        }
    }
    


}
//KS: 线框模型渲染器 
void Rst::Rasterizer::RasterizeTriangleLine(const Triangle& t)
{
    DrawLine(t.c(), t.a(),Rst::Color.red);
    DrawLine(t.c(), t.b(),Rst::Color.blue);
    DrawLine(t.b(), t.a(), Rst::Color.green);
}


//KS: bresenham's line 
void Rst::Rasterizer::DrawLine(Eigen::Vector4f begin, Eigen::Vector4f end, Eigen::Vector3f color)
{
    auto x1 = begin.x();
    auto y1 = begin.y();
    auto x2 = end.x();
    auto y2 = end.y();

   
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;

    dx = x2 - x1;
    dy = y2 - y1;
    dx1 = fabs(dx);
    dy1 = fabs(dy);
    //KS:  优化为整数运算, 不用除法,将k拆为k=dy/dx
    //KS:  px为e0值, e0的时候x0=y0=b=0带入公式:dx(d1-d2)= dy(xi+1)-2yi*dx + (2b-1)*dx
    px = 2 * dy1 - dx1;
    py = 2 * dx1  - dy1;
    if (dy1 <= dx1)//KS: 保证k小于+-45度
    {
        if (dx >= 0)
        {
            x = x1;
            y = y1;
            xe = x2;
        }
       

        else
        {
            x = x2;
            y = y2;
            xe = x1;

        }
        //KS: e0递归初始值
        Eigen::Vector3f point = Eigen::Vector3f(x, y, 1.0f);
        SetPixel(point, color);
        for (i = 0; x < xe; i++)
        {
            x = x + 1;
            if (px < 0)
            {
                px = px + 2 * dy1;
            }
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))//KS: 判断斜率为正还是负 
                {
                    y = y + 1;

                }
                else
                {
                    y = y - 1;
                }
                px = px + 2 * (dy1 - dx1);
            }
            Eigen::Vector3f point = Eigen::Vector3f(x, y, 1.0f);
            SetPixel(point, color);
        }

    }
    else //KS: 大于45度到135度 
    {
        if (dy >= 0)
        {
            x = x1;
            y = y1;
            ye = y2;
        }
        else
        {
            x = x2;
            y = y2;
            ye = y1;
        }
        Eigen::Vector3f point = Eigen::Vector3f(x, y, 1.0f);
        SetPixel(point, color);
        for (i = 0; y < ye; i++)
        {
            y = y + 1;
            if (py <= 0)
            {
                py = py + 2 * dx1;
            }
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                {
                    x = x + 1;
                }
                else
                {
                    x = x - 1;
                }
                py = py + 2 * (dx1 - dy1);
            }
            Eigen::Vector3f point = Eigen::Vector3f(x, y, 1.0f);
            SetPixel(point, color);
        }
    }
}

//KS:  mvp矩阵设置
void Rst::Rasterizer::SetModel(float rotationX, float rotationY, float rotationZ)//KS:  旋转矩阵
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
    //KS: 而观察矩阵是相机本身变换的逆变换 
    Eigen::Matrix4f temp = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f translate;
    translate <<
        1, 0, 0, -eyePos[0],
        0, 1, 0, -eyePos[1],
        0, 0, 1, -eyePos[2],
        0, 0, 0, 1;
    view = translate * temp;//KS: set view matrix 

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

    float halve = eyeFov / 2 * MY_PI / 180;
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

