#pragma once

#include <DirectXMath.h>

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

public:
    DirectX::XMVECTOR upVector;
    DirectX::XMVECTOR forwardVector;
    DirectX::XMVECTOR rightVector;

    Transform();

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

    float Distance(const DirectX::XMVECTOR& target);
};
