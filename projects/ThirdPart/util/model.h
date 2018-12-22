#ifndef _MODEL_H
#define _MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <util/imageutil.h>

#include <assimp/include/Importer.hpp>
#include <assimp/include/scene.h>
#include <assimp/include/postprocess.h>

#include <util/mesh.h>
#include <util/shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class CModel
{
public:
	vector<Texture> textures_loaded;
	vector<CMesh> meshes;
	string directory;
	bool gammaCorrection;

	CModel(string const &path, bool gamma = false) : gammaCorrection(gamma) {
		loadModel(path);
	}

	void Draw(CShader shader) {
		for (unsigned int i = 0; i < meshes.size(); ++i) {
			meshes[i].Draw(shader);
		}
	}

private:
	
	void loadModel(string const &path) {
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode *node, const aiScene *scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			processNode(node->mChildren[i], scene);
		}
	}

	CMesh processMesh(aiMesh *mesh, const aiScene *scene) {
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			Vertex vertex;
			glm::vec3 vector;
			// position
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			// normal
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			// texture Coordinates
			if (mesh->mTextureCoords[0]) {	// 纹理坐标不一定存在
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else {
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}

			/*
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			vertices.push_back(vertex);
			*/

			vertices.push_back(vertex);
		}

		// faces
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}

		// materials
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			//std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			//textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 4. height maps
			std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		}

		return CMesh(vertices, indices, textures);
	}

	vector<Texture> loadMaterialTextures(aiMaterial *material, aiTextureType type, string typeName) {
		vector<Texture> textures;
		for (unsigned int i = 0; i < material->GetTextureCount(type); ++i) {
			aiString str;
			material->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); ++j) {
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip) {
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}
		}
		return textures;
	}
};

#endif // !_MODEL_H
