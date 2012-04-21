#include "Mesh.h"
#include "IRenderer.h"
#include "IMeshBase.h"
#include <iostream>
#include <stdexcept>

Mesh::Mesh(IRenderer& renderer, const ::duke::protocol::Mesh& mesh) {
    const std::string name(mesh.name());

    resource::tryGet(renderer.resourceCache, name, m_pMeshBase);

    if (!m_pMeshBase) {
        const ::duke::protocol::VertexBuffer& vb = mesh.vertexbuffer();
        std::vector<IMeshBase::TVertex> vertices;
        for (int vertexIndex = 0; vertexIndex < vb.vertex_size(); vertexIndex++) {
            const ::duke::protocol::Vertex& vertex = vb.vertex(vertexIndex);
            IMeshBase::TVertex vertexStruct;
            vertexStruct.x = vertex.data(0);
            vertexStruct.y = vertex.data(1);
            vertexStruct.z = vertex.data(2);
            vertexStruct.u = vertex.data(3);
            vertexStruct.v = vertex.data(4);
            vertices.push_back(vertexStruct);
        }
        Buffer<IMeshBase::TVertex> vertexBuffer(renderer.createVertexBuffer(vertices.size(), BUF_STATIC, vertices.data()));
        if (mesh.has_indexbuffer()) {
            const ::duke::protocol::IndexBuffer& ib = mesh.indexbuffer();
            const int indicesCount = ib.index_size();
            std::vector<IMeshBase::TIndex> indices;
            indices.reserve(indicesCount);
            for (int i = 0; i < indicesCount; i++)
                indices.push_back(ib.index(i));
            Buffer<IMeshBase::TIndex> indexBuffer(renderer.createIndexBuffer(indicesCount, BUF_STATIC, indices.data()));
            m_pMeshBase.reset(new IMeshBase(Enums::Get(mesh.type()), vertexBuffer, indexBuffer));
        } else
            m_pMeshBase.reset(new IMeshBase(Enums::Get(mesh.type()), vertexBuffer, NULL));

        resource::put(renderer.resourceCache, name, m_pMeshBase);
    }
}

Mesh::~Mesh() {
}

void Mesh::render(IRenderer& renderer) const {
    if (m_pMeshBase)
        m_pMeshBase->render(renderer);
}

