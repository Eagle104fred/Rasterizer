
#ifndef RASTERIZER_OBJ_LOADER_H
#define RASTERIZER_OBJ_LOADER_H
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>


#include <direct.h>
#define OBJ_CONSOLE_OUTPUT

namespace Obj
{
	//KS:构造一个vector2结构体 
	struct Vector2
	{
		Vector2()
		{
			x = 0.0f;
			y = 0.0f;

		}
		Vector2(float _x, float _y)
		{
			x = _x;
			y = _y;
		}
		//KS: 重载==
		bool operator==(const Vector2& other)const
		{
			return (this->x == other.x && this->y == other.y);
		}
		//KS: 重载!=
		bool operator!=(const Vector2& other)const
		{
			return (this->x != other.x && this->y != other.y);
		}
		//KS: +
		Vector2 operator+(const Vector2& right)const
		{
			return Vector2(this->x + right.x, this->y + right.y);
		}
		//KS: -
		Vector2 operator-(const Vector2& right)const
		{
			return Vector2(this->x - right.x, this->y - right.y);
		}
		
		//KS: *
		Vector2 operator*(const Vector2& other)const
		{
			return Vector2(this->x * other.x, this->y * other.y);
		}
		

		float x;
		float y;
	};
	//KS: vector3类重载
	struct Vector3{
		Vector3() {
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		};
		Vector3(float _x, float _y, float _z)
		{
			x = _x;
			y = _y;
			z = _z;
		}
		bool operator==(Vector3 other)const
		{
			return (this->x == other.x && this->y == other.y && this->z == other.z);
		}
		bool operator!=(Vector3 other)const
		{
			return (this->x != other.x && this->y != other.y && this->z != other.z);
		}
		Vector3 operator+(Vector3 right)const
		{
			return Vector3(this->x + right.x , this->y != right.y , this->z != right.z);
		}
		Vector3 operator-(Vector3 right)const
		{
			return Vector3(this->x - right.x, this->y - right.y, this->z - right.z);
		}
		Vector3 operator*(float other)const 
		{
			return Vector3(this->x * other, this->y * other, this->z * other);
		}
		Vector3 operator/(float other)const
		{
			return Vector3(this->x / other, this->y / other, this->z/other);
		}
		float x;
		float y;
		float z;
	};

	//KS: 顶点类 位置, 法线, textureCoordinate
	struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
		Vector2 TextureCoordinate;
	};
	//KS: 材质类
	struct Material
	{
		Material()
		{
			name;
			Ns = 0.0f;
			Ni = 0.0f;
			d = 0.0f;
			illum = 0;
		}
		std::string name;
		//KS: Ambent Color(材质自发光) 
		Vector3 Ka;
		//KS: Diffuse漫反射
		Vector3 Kd;
		//KS: Specular高光
		Vector3 Ks;
		// Specular Exponent 镜面指数
		float Ns;
		//KS: Optical Density光密度
		float Ni;
		// Dissolve 溶解
		float d;
		// Illumination 照明
		int illum;
		// Ambient Texture Map
		std::string map_Ka;
		// Diffuse Texture Map
		std::string map_Kd;
		// Specular Texture Map
		std::string map_Ks;
		// Specular Hightlight Map
		std::string map_Ns;
		// Alpha Texture Map
		std::string map_d;
		// Bump Map
		std::string map_bump;
	};
	//KS: mesh类
	struct Mesh
	{
		Mesh() {

		}
		Mesh(std::vector<Vertex>& _Vertices, std::vector<unsigned int>& _Indices)
		{
			vertices = _Vertices;
			indices = _Indices;
		}
		//KS: mesh名字
		std::string meshName;
		//KS: 顶点list
		std::vector<Vertex> vertices;
		//KS: 索引list
		std::vector<unsigned int> indices;

		//KS: 材质 
		Material MeshMaterial;
	};
	//KS: 数学库
	namespace math
	{
		Vector3 Cross3(const Vector3 a, const Vector3 b)
		{
			return Vector3(
				a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x);							
		}
		float DotV3(const Vector3 a, const Vector3 b)
		{
			return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
		}

		//KS: 求三维点间距离 
		float MagnitudeV3(const Vector3 in)
		{
			return sqrtf(powf(in.x, 2 + powf(in.y, 2) + powf(in.z, 2)));
		}
		//KS: 计算向量间夹角 
		float AngleBetweenV3(const Vector3 a, const Vector3 b)
		{
			float angle = DotV3(a, b);
			angle /= (MagnitudeV3(a) * MagnitudeV3(b));
			return angle = acosf(angle);
		}
		
		//KS:向量投影
		Vector3 ProjV3(const Vector3 a, const Vector3 b)
		{
			Vector3 bn = b / MagnitudeV3(b);
			return bn = bn * DotV3(a, bn);
		}
	}

	//KS:  算法库
	namespace Algorithm
	{
		Vector3 operator*(const float left, const Vector3& right)
		{
			return Vector3(right.x * left, right.y * left, right.z * left);
		}
		// A test to see if P1 is on the same side as P2 of a line segment ab
		bool SameSide(Vector3 p1, Vector3 p2, Vector3 a, Vector3 b)
		{
			Vector3 cp1 = math::Cross3(b - a, p1 - a);
			Vector3 cp2 = math::Cross3(b - a, p2 - a);
			if (math::DotV3(cp1, cp2) >= 0)
				return true;
			else
				return false;
		}
		//KS: 为一个三角形生成法线 
		Vector3 GenTriNromal(Vector3 t1, Vector3 t2, Vector3 t3)
		{
			Vector3 u = t2 - t1;
			Vector3 v = t3 - t1;
			Vector3 normal = math::Cross3(u, v);
			return normal;
		}
		bool InTriangle(Vector3 point, Vector3 tri1, Vector3 tri2, Vector3 tri3)
		{
			//KS: 先校验点是否在三角形所形成的三角柱内
			bool withinPrism = 
				SameSide(point, tri1, tri2, tri3) && 
				SameSide(point, tri2, tri1, tri3) && 
				SameSide(point ,tri3, tri1, tri2);
			if (!withinPrism)
				return false;
			//KS: 然后校验点是否与三角形共面(点作垂线到法线并测量距离如果本来就在三角形上距离为0)
			Vector3 n = GenTriNromal(tri1, tri2, tri3);
			Vector3 proj = math::ProjV3(point, n);
			if (math::MagnitudeV3(proj) == 0)
				return true;
			else
				return false;
		}

		//KS: 字符串分割用于读取obj
		inline void Split(const std::string& in, std::vector<std::string>& out, std::string token)
		{
			out.clear();
			std::string temp;
			for (int i = 0; i < int(in.size()); i++)
			{
				std::string test = in.substr(i, token.size());
				if (test == token)
				{
					
					if (!temp.empty())
					{
						out.push_back(temp);
						temp.clear();
						i += (int)token.size() - 1;
					}
					else
					{
						out.push_back("");
					}					
				}
				else if (i + token.size() >= in.size())//KS: 剪枝,遍历到末尾, 如果后面字符不足一个token提前结束 
				{
					temp += in.substr(i, token.size());
					out.push_back(temp);
					break;
				}
				else
				{
					temp += in[i];
				}
			}
		}

		inline std::string FirstToken(const std::string& in)
		{
			if (!in.empty())
			{
				size_t tokenStart = in.find_first_not_of(" \t");//KS: 找出不是"/t"的第一个字符的位置 
				size_t tokenEnd = in.find_first_of(" \t", tokenStart);//KS:找到下一个"/t"字符的位置  
				if (tokenStart != std::string::npos && tokenEnd != std::string::npos)
				{
					return in.substr(tokenStart, tokenEnd - tokenStart);
				}
				else if (tokenStart != std::string::npos)
				{
					return in.substr(tokenStart);
				}
				
			}
			return "";
		}
		//KS: 拿到token后的所有字符 
		inline std::string Tail(const std::string& in)
		{
			size_t tokenStart = in.find_first_not_of(" \t");
			size_t spaceStart = in.find_first_of(" \t",tokenStart);
			size_t tailStart = in.find_first_not_of(" \t",spaceStart);
			size_t tailEnd = in.find_last_not_of(" \t");
			if (tailStart != std::string::npos && tailEnd != std::string::npos)
			{
				return in.substr(tailStart, tailEnd - tailStart);

			}
			else if(tailStart!=std::string::npos)
			{
				return in.substr(tailStart);//KS:  get from tailStart to the end
			}
		}

		// Get element at given index position 
		template<typename T>
		inline const T& GetElement(const std::vector<T>& elements, std::string& index)
		{
			int ind = std::stoi(index);
			if (ind < 0)
			{
				ind = int(elements.size()) + ind;
			}
			else
			{
				ind--;
			}
			return elements[ind];
		}
	}

	//KS: OBJ Model Loader 
	class Loader
	{
	public:
		Loader()
		{

		}
		~Loader()
		{

		}
		bool LoadFile(std::string Path)
		{
			if (Path.substr(Path.size() - 4, 4) != ".obj")
				return false;
			std::ifstream file(Path);
			if (!file.is_open())
				return false;
				
			std::vector<Vector3> _positions;
			std::vector<Vector2> _texCoords;
			std::vector<Vector3> _normals;
			std::vector<Vertex> _vertices;
			std::vector<unsigned int> _indices;

			std::vector<std::string> MeshMatNames;

			bool listening = false;
			std::string _meshName;

			Mesh tempMesh;

#ifdef OBJ_CONSOLE_OUTPUT
			const unsigned int outputEveryNth = 1000;
			unsigned int ouputIndicator = outputEveryNth;
#endif//OBJ_CONSOLE_OUTPUT
			std::string curLine;
			while (std::getline(file, curLine))
			{
#ifdef OBJ_CONSOLE_OUTPUT
				if (ouputIndicator = ((ouputIndicator + 1) % outputEveryNth) == 1)//KS: while中只输出一次, ???为什么要用这么复杂的机制??? 
				{
					if (!_meshName.empty())
					{
						std::cout << "\r- " << _meshName
							<< "\t| vertices > " << _positions.size()
							<< "\t| texcoords > " << _texCoords.size()
							<< "\t| normals > " << _normals.size()
							<< "\t| triangles > " << (_vertices.size() / 3)
							<<(!MeshMatNames.empty()?"\t| material: "+ MeshMatNames.back() :"");
					}
				}
#endif // OBJ_CONSOLE_OUTPUT

				// Generate a Mesh Object or Prepare for an object to be created

				auto firstToken = Algorithm::FirstToken(curLine);
				//if (Algorithm::FirstToken(curLine) == "o" || Algorithm::FirstToken(curLine) == "g" || curLine[0] == 'g')
				if (firstToken == "o" || firstToken == "g" || curLine[0] == 'g')
				{
					if (!listening)
					{
						listening = true;
						if (Algorithm::FirstToken(curLine) == "o" || Algorithm::FirstToken(curLine) == "g")
						{
							_meshName = Algorithm::Tail(curLine);
						}
						else
						{
							_meshName = "unnamed";
						}
					}
					else
					{
						//KS: 生成mesh并放入array
						if (!_indices.empty() && !_vertices.empty())
						{
							//KS: create Mesh
							tempMesh = Mesh(_vertices, _indices);
							tempMesh.meshName = _meshName;

							//KS: Insert Mesh
							loadedMeshesList.push_back(tempMesh);

							_vertices.clear();
							_indices.clear();
							_meshName.clear();

							_meshName = Algorithm::Tail(curLine);//KS: ???? 

						}
						else
						{
							//KS: ????????????????????????????? 
							//if (Algorithm::FirstToken(curLine) == "o" || Algorithm::FirstToken(curLine) == "g")
							if (firstToken == "o" || firstToken == "g")
							{
								_meshName = Algorithm::Tail(curLine);
							}
							else
							{
								_meshName = "unnamed";
							}
						}
					}
#ifdef OBJ_CONSOLE_OUTPUT
					std::cout << std::endl;
					ouputIndicator = 0;
#endif // OBJ_CONSOLE_OUTPUT

				}
				// Generate a Vertex Position
				//if (Algorithm::FirstToken(curLine) == "v")//KS: 读取顶点位置 
				if (firstToken == "v" )
				{
					std::vector<std::string>spos;
					Vector3 vpos;
					Algorithm::Split(Algorithm::Tail(curLine), spos, " ");
					vpos.x = std::stof(spos[0]);
					vpos.y = std::stof(spos[1]);
					vpos.z = std::stof(spos[2]);

					_positions.push_back(vpos);
				}
				// Generate a Vertex Texture Coordinate
				//if (Algorithm::FirstToken(curLine) == "vt")//KS: 顶点贴图坐标
				if (firstToken =="vt")
				{
					std::vector<std::string>stex;
					Vector2 vtex;
					Algorithm::Split(Algorithm::Tail(curLine), stex, " ");
					vtex.x = std::stof(stex[0]);
					vtex.y = std::stof(stex[1]);
					_texCoords.push_back(vtex);
				}
				// Generate a Vertex Normal;
				//if (Algorithm::FirstToken(curLine) == "vn")//KS: 顶点法线坐标
				if (firstToken == "vn")
				{
					std::vector<std::string>sn;
					Vector3 vn;
					Algorithm::Split(Algorithm::Tail(curLine),sn," ");
					vn.x = std::stof(sn[0]);
					vn.y = std::stof(sn[1]);
					vn.z = std::stof(sn[2]);
					_normals.push_back(vn);
				}
				// Generate a Face (vertices & indices)
				//if (Algorithm::FirstToken(curLine) == "f")
				if (firstToken == "f")
				{
					std::vector<Vertex>vVerts;
					GenVertivesFromRawOBJ(vVerts, _positions, _texCoords, _normals, curLine);
					// Add Vertices
					for (int i = 0; i < int(vVerts.size()); i++)
					{
						_vertices.push_back(vVerts[i]);
						loadedVerticesList.push_back(vVerts[i]);

					}
					std::vector<unsigned int>tempIndices;
								
					VertexTriangluation(tempIndices, vVerts);
								
					// Add Indices
					for (int i = 0; i < int(tempIndices.size()); i++)
					{
						unsigned int indnum = (unsigned int)((_vertices.size()) - vVerts.size()) + tempIndices[i];
						_indices.push_back(indnum);

						indnum = (unsigned int)((loadedVerticesList.size()) - vVerts.size()) + tempIndices[i];
						loadedIndicesList.push_back(indnum);
					}
				}

				// Get Mesh Material Name
				if (Algorithm::FirstToken(curLine) == "usemtl")
				{
					MeshMatNames.push_back(Algorithm::Tail(curLine));

					// Create new Mesh, if Material changes within a group
					if (!_indices.empty() && !_vertices.empty())
					{
						tempMesh = Mesh(_vertices, _indices);
						tempMesh.meshName = _meshName;
						int i = 2;
						while (1) 
						{
							tempMesh.meshName = _meshName + "_" + std::to_string(i);
							for (auto &m : loadedMeshesList)
							{
								if (m.meshName == tempMesh.meshName)
									continue;
							}
							break;
						}

						// Insert Mesh
						loadedMeshesList.push_back(tempMesh);

						// Cleanup
						_vertices.clear();
						_indices.clear();

					}
#ifdef OBJ_CONSOLE_OUTPUT
					ouputIndicator = 0;
#endif
				}
				// Load Materials
				if (Algorithm::FirstToken(curLine) == "mtllib")
				{
					std::vector<std::string>temp;
					Algorithm::Split(Path, temp, "/");
					std::string pathToMat = "";
					if (temp.size() != 1)
					{
						for (int i = 0; i < temp.size() - 1; i++)
						{
							pathToMat += temp[i] + "/";
						}
					}

					pathToMat += Algorithm::Tail(curLine);
#ifdef OBJ_CONSOLE_OUTPUT
					std::cout << std::endl << "- find materials in: " << pathToMat << std::endl;
#endif
					//Load Materials
					LoadMaterials(pathToMat);
				}
			}
#ifdef OBJ_CONSOLE_OUTPUT
			std::cout  << std::endl;
#endif

			// Deal with last mesh
			if (!_indices.empty() && !_vertices.empty())
			{
				tempMesh = Mesh(_vertices, _indices);
				tempMesh.meshName = _meshName;

				loadedMeshesList.push_back(tempMesh);
			}
			file.close();

			for (int i = 0; i < MeshMatNames.size();i++)
			{
				std::string matName = MeshMatNames[i];
				// Find corresponding material name in loaded materials
				// when found copy material variables into mesh material
				for (int j = 0; j < loadedMaterialsList.size(); j++)
				{
					if (loadedMaterialsList[j].name == matName)
					{
						loadedMeshesList[i].MeshMaterial = loadedMaterialsList[j];
						break;
					}
				}
			}
			if (loadedMeshesList.empty() && loadedVerticesList.empty() && loadedIndicesList.empty())
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		// Loaded Mesh Objects
		std::vector<Mesh> loadedMeshesList;
		// Loaded Vertex Objects
		std::vector<Vertex> loadedVerticesList;
		// Loaded Index Positions
		std::vector<unsigned int> loadedIndicesList;
		// Loaded Material Objects
		std::vector<Material> loadedMaterialsList;

	private:

		//KS:  生成顶点
		void GenVertivesFromRawOBJ(std::vector<Vertex>& oVerts, 
			const std::vector<Vector3>& iPositions, 
			const std::vector<Vector2>& iTexCoords, 
			const std::vector<Vector3>& iNormals, 
			std::string icurLine)
		{
			std::vector<std::string>sface, svert;
			Vertex vVert;
			Algorithm::Split(Algorithm::Tail(icurLine), sface, " ");
			bool noNormal = false;

			for (int i = 0; i < int(sface.size()); i++)
			{

				int vtype;//KS: vertex的类型 
				Algorithm::Split(sface[i], svert, "/");

				if (svert.size() == 1)
				{
					vtype = 1;//KS: 只有位置信息 
				}
				if (svert.size() == 2)
				{
					vtype = 2;//KS: 有位置和文理信息 
				}

				if (svert.size() == 3)
				{
					if (svert[1] != "")
					{
						vtype = 4;//KS: 位置, 文理, 法线 
					}
					else
					{
						vtype = 3;//KS: 位置 法线 
					}
				}

				// Calculate and store the vertex
				switch (vtype)
				{
					case 1: // P
					{
						vVert.Position = Algorithm::GetElement(iPositions, svert[0]);
						vVert.TextureCoordinate = Vector2(0, 0);
						noNormal = true;
						oVerts.push_back(vVert);
						break;

					}
					case 2:// P/T
					{
						vVert.Position = Algorithm::GetElement(iPositions, svert[0]);
						vVert.TextureCoordinate = Algorithm::GetElement(iTexCoords, svert[1]);
						noNormal = true;
						oVerts.push_back(vVert);
						break;
					}
					case 3:// P//N
					{
						vVert.Position = Algorithm::GetElement(iPositions, svert[0]);
						vVert.TextureCoordinate = Vector2(0, 0);
						vVert.Normal = Algorithm::GetElement(iNormals, svert[2]);
						oVerts.push_back(vVert);
						break;															
					}
					case 4: // P/T/N
					{
						vVert.Position = Algorithm::GetElement(iPositions, svert[0]);
						vVert.TextureCoordinate = Algorithm::GetElement(iTexCoords, svert[1]);
						vVert.Normal = Algorithm::GetElement(iNormals, svert[2]);
						oVerts.push_back(vVert);
						break;
					}
					default:
					{
						break;
					}

				}
						
			}
			//KS: 补上法线 
			if (noNormal)
			{
				Vector3 A = oVerts[0].Position - oVerts[1].Position;
				Vector3 B = oVerts[2].Position - oVerts[1].Position;
				Vector3 normal = math::Cross3(A, B);
				for (int i = 0; i < int(oVerts.size()); i++)
				{
					oVerts[i].Normal = normal;
				}
			}
		}

		//KS: 顶点三角化 ,生成有效点的索引
		void VertexTriangluation(std::vector<unsigned int>& oIndices,
			const std::vector<Vertex>& iVerts)
		{
			//KS: 顶点数少于3, 无法构成三角形 
			if (iVerts.size() < 3)
			{
				return;
			}
			//KS:  顶点数等于3, 可以构成一个三角形, 无需计算
			if (iVerts.size() == 3)
			{
				oIndices.push_back(0);
				oIndices.push_back(1);
				oIndices.push_back(2);
				return; 
			}
			std::vector<Vertex>tVerts = iVerts;
			while (true)
			{
				for (int i = 0; i < int(tVerts.size()); i++)
				{
					// pPrev = the previous vertex in the list
					Vertex pPrev;
					if (i == 0)
					{
						pPrev = tVerts[tVerts.size() - 1];
					}
					else
					{
						pPrev = tVerts[i - 1];

					}
					// pCur = the current vertex;
					Vertex pCur = tVerts[i];
					// pNext = the next vertex in the list
					Vertex pNext;
					if (i == tVerts.size() - 1)
					{
						pNext = tVerts[0];//KS: 如果循环到最后返回第一个vert 
					}
					else
					{
						pNext = tVerts[i + 1];
					}

					// Check to see if there are only 3 verts left
					// if so this is the last triangle
					if (tVerts.size() == 3)
					{
						// Create a triangle from pCur, pPrev, pNext
						for (int j = 0; j < int(tVerts.size()); j++)
						{
							if (iVerts[j].Position == pCur.Position)
								oIndices.push_back(j);
							if (iVerts[j].Position == pPrev.Position)
								oIndices.push_back(j);
							if (iVerts[j].Position == pNext.Position)
								oIndices.push_back(j);
						}
						tVerts.clear();
						break;
					}
							 
					if (tVerts.size() == 4)
					{
						// Create a triangle from pCur, pPrev, pNext
						for (int j = 0; j < int(iVerts.size()); j++)
						{
							if (iVerts[j].Position == pCur.Position)
								oIndices.push_back(j);
							if (iVerts[j].Position == pPrev.Position)
								oIndices.push_back(j);
							if (iVerts[j].Position == pNext.Position)
								oIndices.push_back(j);
						}

						Vector3 tempVec;
						for (int  j = 0; j < int(tVerts.size()); j++)
						{
							if (tVerts[j].Position != pCur.Position
								&&tVerts[j].Position!=pPrev.Position
								&&tVerts[j].Position!=pNext.Position)
							{
								tempVec = tVerts[j].Position;
								break;
							}
						}
						for (int j = 0; j < int(iVerts.size()); j++)
						{
							if (iVerts[j].Position == pPrev.Position)
								oIndices.push_back(j);
							if (iVerts[j].Position == pNext.Position)
								oIndices.push_back(j);
							if (iVerts[j].Position == tempVec)
								oIndices.push_back(j);
						}
						tVerts.clear();
						break;
					}

					// If Vertex is not an interior vertex
					//KS: 不是内部顶点 
					float angle = math::AngleBetweenV3(pPrev.Position - pCur.Position, pNext.Position - pCur.Position);
					angle *= (180 / 3.14159265359);
					if (angle <= 0 && angle >= 180)
						continue;

					// If any vertices are within this triangle
					//KS: 顶点在三角形内 
					bool inTri = false;
					for (int j = 0; j < int(iVerts.size()); j++)
					{
						if (Algorithm::InTriangle(iVerts[j].Position, pPrev.Position, pCur.Position, pNext.Position)
							&& iVerts[j].Position != pPrev.Position
							&& iVerts[j].Position != pCur.Position
							&& iVerts[j].Position != pNext.Position)
						{
							inTri = true;
							break;
						}
					}
					if (inTri)//KS: 如果顶点在已构建的三角形内就忽略该点 
						continue;

					// Create a triangle from pCur, pPrev, pNext
					for (int j = 0; j < int(iVerts.size()); j++)
					{
						if (iVerts[j].Position == pPrev.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pNext.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pCur.Position)
							oIndices.push_back(j);
					}

					// Delete pCur from the list
					for (int j = 0; j < int(tVerts.size()); j++)
					{
						if (tVerts[j].Position == pCur.Position)
						{
							tVerts.erase(tVerts.begin() + j);
							break;
						}

					}
					i = -1;

				}
				// if no triangles were created
				if (oIndices.size() == 0)
					break;

				// if no more vertices
				if (tVerts.size() == 0)
					break;
			}
		}

		bool LoadMaterials(std::string path)
		{
			// If the file is not a material file return false
			if (path.substr(path.size() - 4, path.size()) != ".mtl")
			{
				return false;
			}

			std::ifstream file(path);

			if (!file.is_open())
			{
				return false;
			}

			Material tempMaterial;

			bool listening = false;

			std::string curLine;
			while (std::getline(file,curLine))
			{
				if (Algorithm::FirstToken(curLine) == "newmtl")
				{
					if (!listening)
					{
						listening = true;
						if (curLine.size() > 7)
						{
							tempMaterial.name = Algorithm::Tail(curLine);
						}
						else
						{
							tempMaterial.name = "none";
						}
					}
					else
					{
						// Push Back loaded Material
						loadedMaterialsList.push_back(tempMaterial);
						// Clear Loaded Material
						tempMaterial = Material();

						if (curLine.size() > 7)
						{
							tempMaterial.name = Algorithm::Tail(curLine);
						}
						else
						{
							tempMaterial.name = "none";
						}
					}
				}
				// Ambient Color
				if (Algorithm::FirstToken(curLine) == "Ka")
				{
					std::vector<std::string>temp;
					Algorithm::Split(Algorithm::Tail(curLine), temp, " ");
					if (temp.size() != 3)
						continue;
					tempMaterial.Ka.x = std::stof(temp[0]);
					tempMaterial.Ka.y = std::stof(temp[1]);
					tempMaterial.Ka.z = std::stof(temp[2]);
				}
				// Diffuse Color
				if (Algorithm::FirstToken(curLine) == "Kd")
				{
					std::vector<std::string>temp;
					Algorithm::Split(Algorithm::Tail(curLine), temp, " ");

					if (temp.size() != 3)
					{
						continue;
					}
					tempMaterial.Kd.x = std::stof(temp[0]);
					tempMaterial.Kd.y = std::stof(temp[1]);
					tempMaterial.Kd.z = std::stof(temp[2]);
				}
				//Specular Color
				if (Algorithm::FirstToken(curLine) == "Ks")
				{
					std::vector<std::string>temp;
					Algorithm::Split(Algorithm::Tail(curLine), temp, " ");
					if (temp.size() != 3)
						continue;
					tempMaterial.Ks.x = std::stof(temp[0]);
					tempMaterial.Ks.y = std::stof(temp[1]);
					tempMaterial.Ks.z = std::stof(temp[2]);

				}
				// Specular Exponent
				if (Algorithm::FirstToken(curLine) == "Ns")
				{
					tempMaterial.Ns = std::stof(Algorithm::Tail(curLine));
				}
				// Optical Density
				if(Algorithm::FirstToken(curLine) == "Ni")
				{
					tempMaterial.Ni = std::stof(Algorithm::Tail(curLine));
				}
				//Dissolve
				if (Algorithm::FirstToken(curLine) == "d")
				{
					tempMaterial.d = std::stof(Algorithm::Tail(curLine));
				}
				//Illumination
				if (Algorithm::FirstToken(curLine) == "illum")
				{
					tempMaterial.illum = std::stof(Algorithm::Tail(curLine));
				}
				//Ambient Texture Map
				if (Algorithm::FirstToken(curLine) == "map_Ka")
				{
					tempMaterial.map_Ka = std::stof(Algorithm::Tail(curLine));
				}
				// Diffuse Texture Map
				if (Algorithm::FirstToken(curLine) == "map_Kd")
				{
					tempMaterial.map_Kd = std::stof(Algorithm::Tail(curLine));
				}
				//Specular Texture Map
				if (Algorithm::FirstToken(curLine) == "map_Ks")
				{
					tempMaterial.map_Ks = std::stof(Algorithm::Tail(curLine));
				}
				//Specular Hightlight Map
				if (Algorithm::FirstToken(curLine) == "map_Ns")
				{
					tempMaterial.map_Ns = std::stof(Algorithm::Tail(curLine));
				}
				//Alpha Texture Map
				if (Algorithm::FirstToken(curLine) == "map_d")
				{
					tempMaterial.map_d = std::stof(Algorithm::Tail(curLine));
				}
				//Bump Map
				if (Algorithm::FirstToken(curLine) == "map_Bump")
				{
					tempMaterial.map_bump = std::stof(Algorithm::Tail(curLine));
				}
			}
			// Deal with last material

		   // Push Back loaded Material
			loadedMaterialsList.push_back(tempMaterial);
			// Test to see if anything was loaded
			// If not return false
			if (loadedMaterialsList.empty())
				return false;
			else
				return true;
		}
	};
}
#endif // !RASTERIZER_OBJ_LOADER_H

