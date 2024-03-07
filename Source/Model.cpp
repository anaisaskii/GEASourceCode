#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "Model.h"

namespace GE {
	bool Model::loadFromFile(const char* filename) {
		std::vector<Vertex> loadedVertices;

		Assimp::Importer imp;

		const aiScene* pScene = imp.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs);
		
		if (!pScene) {
			return false;
		}
		
		//checks through meshes, faces, vertex
		for (int MeshIdx = 0; MeshIdx < pScene->mNumMeshes; MeshIdx++) {
			const aiMesh* mesh = pScene->mMeshes[MeshIdx];

			for (int faceIdx = 0; faceIdx < mesh->mNumFaces; faceIdx++) {
				const aiFace& face = mesh->mFaces[faceIdx];

				for (int vertIdx = 0; vertIdx < 3; vertIdx++) {
					const aiVector3D* pos = &mesh->mVertices[face.mIndices[vertIdx]];
					const aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[vertIdx]];
					
					loadedVertices.push_back(Vertex(pos->x, pos->y, pos->z, uv.x, uv.y));
				}
			}
		}


		//accessing/storing vertex data for rendering
		numVertices = loadedVertices.size(); //number of vertices loaded

		glGenBuffers(1, &vbo); //create vbo buffer

		glBindBuffer(GL_ARRAY_BUFFER, vbo); //set as array buffer

		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), loadedVertices.data(), GL_STATIC_DRAW); //copy vertex data from loaded vertices to array buffer 

		glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind buffer

		return true;
	}
}