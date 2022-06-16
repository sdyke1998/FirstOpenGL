#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <vector>
#include <shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Mesh.h>

int TextureFromFile(std::string, std::string);

class Model {

public:
	Model(char* path) {

		loadModel(path);
	}
	void Draw(Shader& shader);

private:
	std::vector<Mesh> meshes;
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> loadMaterialTextures(
		aiMaterial *mat, aiTextureType type, std::string typeName
	);
};

void Model::Draw(Shader& shader) {

	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(shader);
	}
}

void Model::loadModel(std::string path) {

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {

		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {

		Vertex vertex;

		glm::vec3 position = glm::vec3(
			mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z
		);
		vertex.Position = position;

		glm::vec3 normal = glm::vec3(
			mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z
		);
		vertex.Normal = normal;

		if (mesh->mTextureCoords[0]) {

			glm::vec2 texCoord = glm::vec2(
				mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y
			);
			vertex.TexCoords = texCoord;
		}
		else vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {

		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0) {

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> texturesLoaded;
std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {

	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {

		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
			
			if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) {

				textures.push_back(texturesLoaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {

			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.path = str.C_Str();
			textures.push_back(texture);
		}
	}
	return textures;
}

int TextureFromFile(std::string texture_str, std::string directory) {

	return 0;
}


#endif