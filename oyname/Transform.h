#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "gdxutil.h"

enum class Space {
    Local,
    World
};

class Transform 
{
private:
    DirectX::XMVECTOR position;
    DirectX::XMVECTOR lookAt;
    DirectX::XMVECTOR up;
    DirectX::XMVECTOR right;

    DirectX::XMMATRIX rotation;
    DirectX::XMMATRIX translation;
    DirectX::XMMATRIX scaling;

    DirectX::XMMATRIX* worldMatrix;

    const DirectX::XMVECTOR forwardVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    const DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    const DirectX::XMVECTOR rightVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    float ExtractRoll(const DirectX::XMMATRIX* XMMatrix_p_Rotation);

    DirectX::XMMATRIX GetLocalTransformationMatrix();

public:

    Transform();

    // Setter
    void setWorldMatrix(DirectX::XMMATRIX* world) { worldMatrix = world; }

    // Getter

    const DirectX::XMVECTOR getPosition() const { return position; }
                                      
    const DirectX::XMVECTOR getLookAt() const { return lookAt; }
                                        
    const DirectX::XMVECTOR getUp() const { return up; }
                                      
    const DirectX::XMVECTOR getRight() const { return right; }
                                        
    const DirectX::XMMATRIX getRotation() const { return rotation; }
                                     
    const DirectX::XMMATRIX getTranslation() const { return translation; }
                                     
    const DirectX::XMMATRIX getScaling() const { return scaling; }

    //

    void Rotate(float fRotateX, float fRotateY, float fRotateZ, Space space = Space::Local);

    void Turn(float fRotateX, float fRotateY, float fRotateZ, Space space = Space::Local);

    void Position(float x, float y, float z);

    void Move(float x, float y, float z, Space space = Space::Local);

    void Scale(float x, float y, float z);

    void LookAt(const DirectX::XMVECTOR& target, const DirectX::XMVECTOR& upVector);

    void RotateTowardsVector(const DirectX::XMVECTOR& targetDirection, const DirectX::XMVECTOR& upVector);

    float Distance(const DirectX::XMVECTOR& target);
};
