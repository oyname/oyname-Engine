#include "Entity.h"


Entity::Entity() {}

Entity::~Entity() {}

void Entity::Init(gdx::CGIDX* engine)
{
    m_engine = engine;
}

void Entity::CreateMesh(LPMESH* mesh, BRUSH* lpBrush) 
{
    *mesh = m_engine->GetMM().createMesh();
    m_engine->GetMM().addMeshToBrush(lpBrush, *mesh);

    XMMATRIX mRotation = XMMatrixIdentity();
    XMMATRIX mScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
    XMMATRIX mTranslation = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

    (*mesh)->cb.viewMatrix = m_engine->GetCam().GetCurrentCam()->viewMatrix();
    (*mesh)->cb.projectionMatrix = m_engine->GetCam().GetCurrentCam()->projectionMatrix();

    m_engine->GetBM().CreateBuffer(&(*mesh)->cb, sizeof(ConstantBuffer), 1, D3D11_BIND_CONSTANT_BUFFER, &(*mesh)->constantBuffer);
}