#pragma once

#include <DirectXMath.h>

class Transform
{
public:
    void TurnEntityEx(float fRotateX, float fRotateY, float fRotateZ);
    void TurnEntity(float fRotateX, float fRotateY, float fRotateZ);
    void RotateEntity(float fRotateX, float fRotateY, float fRotateZ);
    void PositionEntity(float x, float y, float z);
    void MoveEntity(float x, float y, float z);


private:
    XMVECTOR position;     // Position des Objekts
    XMVECTOR lookAt;       // Blickrichtung
    XMVECTOR up;           // Up-Vektor
    XMVECTOR right;        // Right-Vektor

    XMMATRIX mRotate;
    XMMATRIX mTranslation;
    XMMATRIX mScaling;
};

