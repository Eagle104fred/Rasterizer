// Rasterizer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include<cmath>
#include<Eigen\Core>
#include <Eigen\Dense>

#include <iostream>
#include"opencv.hpp"
#include"Rasterizer.h"
#include"ObjLoader.h"



int main()
{ 
//KS: Eigen test:
   /* std::cout << "Example of cpp\n" << std::endl;
    float a = 1.0, b = 2.0;
    std::cout << a/b << std::endl;
    //KS: 实现sin(30弧度)
    std::cout << std::sin(30*std::acos(1)/180) << std::endl;

    //KS: 实现vector3fv,w(1,2,3);
    Eigen::Vector3f v(1.0f, 2.0f, 3.0f);
    Eigen::Vector3f w(1.0f, 2.0f, 3.0f);
    //KS: 实现矩阵加法
    std::cout << v+w << std::endl;

    //KS: 矩阵缩放
    std::cout << v*3.0f << std::endl;

    //KS: 矩阵逐个元素定义并输出
    //一个有意思的现象: Eigen的矩阵下标是从上到下, 然后再是左到右排列的
    Eigen::Matrix3f i;
    i << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
    std::cout << i << std::endl;
    */
 //KS: opencv test   
  /*   cv::Mat img = cv::imread("test.jpg");
    imshow("f22", img);
    cv::waitKey(0);*/

    int key = 0;
    int frame_count = 0;
    float angleX = 0, angleY = 0, angleZ = 0;

    //KS: 初始化视窗 
    Rst::Rasterizer r(700, 700);
    //KS: 设置摄像机位置 
    Eigen::Vector3f eyePos = {0,0,10};

/***********作业2\作业1***********/
    //KS: 渲染三角形(作业2\作业1) 
    std::vector<Eigen::Vector3f>pos{
        {2,0,-2},{0,2,-2},{-2,0,-2},
        {3.5, -1, -5},{2.5, 1.5, -5},{-1, 0.5, -5},
        /*三角形法线案例:
        光栅化时只有当法线朝向摄像机是才会被渲染, 
        本框架法线朝向按照三角形input点逆时针螺旋定则,
        把投影矩阵的right加个负号可以看到隐藏的三角形*/
        {2, 0, -2},{-0, 0, -2},{2, 2, -2}
    };
    std::vector<Eigen::Vector3i>ind{ 
        {0,1,2},
        {3, 4, 5},
        {6, 7, 8}, 
    };

    std::vector<Eigen::Vector3f>cols{
        {217.0, 238.0, 185.0},
        {217.0, 238.0, 185.0},
        {217.0, 238.0, 185.0},
        {185.0, 217.0, 238.0},
        {185.0, 217.0, 238.0},
        {185.0, 217.0, 238.0},
        {185.0, 50.0, 238.0},
        {185.0, 50.0, 238.0},
        {185.0, 50.0, 238.0}
    };
    //KS: 输入点坐标和索引 
    auto posId = r.LoadPosition(pos);
    auto indId = r.LoadIndex(ind);
    auto colId = r.LoadColor(cols);
/***********作业2\作业1***********/
    

/***********作业3***********/

    Obj::Loader Loader;
    std::string objPath = "../models/spot/";
    //KS: 加载obj模型
    std::vector<Triangle*> TriangleList;

    bool isLoad = Loader.LoadFile("D:/Games/Games101/Rasterizer/models/spot/spot_triangulated_good.obj");
    for (auto mesh : Loader.loadedMeshesList)
    {
        for (int i = 0; i < mesh.vertices.size(); i += 3)//KS: 三角形循环 
        {
            Triangle* t = new Triangle();
            for (int j = 0; j < 3; j++)//KS: 顶点循环 ,将mesh封装成triangle
            {
                t->SetVertex(j, Eigen::Vector4f(mesh.vertices[i + j].Position.x,
                    mesh.vertices[i + j].Position.y,
                    mesh.vertices[i + j].Position.z, 1.0f));
                t->SetNormal(j, Eigen::Vector3f(mesh.vertices[i + j].Normal.x,
                    mesh.vertices[i + j].Normal.y,
                    mesh.vertices[i + j].Normal.z));
                t->SetTexCoord(j, Eigen::Vector2f(mesh.vertices[i + j].TextureCoordinate.x,
                    mesh.vertices[i + j].TextureCoordinate.y));

            }
            TriangleList.push_back(t);
        }
    }

   //KS: 选择Shader(Model)
    r.SelectShader(Rst::ShaderType::Normal);
/***********作业3***********/
    while (key != 27)//KS: esc退出循环 
    {
        r.Clear(Rst::Buffer::Color | Rst::Buffer::Depth);


        r.SetModel(angleX,angleY,angleZ);
        r.SetView(eyePos);
        r.SetProjection(45.0, 1, 0.1, 50);//KS: fov:45,宽高比:1, Near:0.1,Far:50 


        //KS: 显示不同的案例 
        //r.DrawTriangle(posId, indId,colId,Rst::Primitive::Line);
        r.DrawTriangle(posId, indId, colId, Rst::Primitive::Fill);
        r.DrawModel(TriangleList);



        cv::Mat image(700, 700, CV_32FC3, r.GetFrameBuffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);//KS: 8bit,unsignint, c3=三通道
        cv::imshow("image", image);
        key = cv::waitKey(10);



        //KS: 操纵模型
        if (key == 'a') {
            angleZ += 10;
        }
        else if (key == 'd') {
            angleZ -= 10;
        }
        else if (key == 'w') {
            angleX += 10;
        }
        else if (key == 's') {
            angleX -= 10;
        }
        else if (key == 'q') {
            angleY += 10;
        }
        else if (key == 'e') {
            angleY -= 10;
        }
        else if (key == 'p') {
            eyePos[2] -= 1;
        }
        else if (key == 'l') {
            eyePos[2] += 1;
        }
    }

}
