#pragma once
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "gdxutil.h"
using namespace DirectX;

enum class Space {
    Local,
    World
};

class Transform
{
private:
    // INTERNE SPEICHERUNG MIT QUATERNIONEN
    XMVECTOR position;          // W=1.0f
    XMVECTOR rotationQuat;      // Quaternion (X, Y, Z, W)
    XMVECTOR scale;             // W=0.0f

    // GECACHTE MATRIZEN (mutable für const Getter)
    mutable XMMATRIX rotationMatrix;
    mutable XMMATRIX translationMatrix;
    mutable XMMATRIX scalingMatrix;
    mutable bool matricesDirty;

    // EXTERNE REFERENZ (nicht mutable - nur zum Schreiben)
    XMMATRIX* worldMatrix;

    // CACHED DIRECTION VECTORS (mutable für const Getter)
    mutable XMVECTOR lookAt;
    mutable XMVECTOR up;
    mutable XMVECTOR right;
    mutable bool vectorsDirty;

    // PRIVATE METHODEN
    void updateMatrices() const;
    void updateDirectionVectors() const;
    XMVECTOR eulerToQuaternion(float pitch, float yaw, float roll) const;
    void quaternionToEuler(const XMVECTOR& quat, float& pitch, float& yaw, float& roll) const;

public:
    // =========== KONSTRUKTOREN ===========
    Transform();
    explicit Transform(XMMATRIX* world);
    ~Transform();

    // =========== EXISTIERENDE API ===========
    void setWorldMatrix(XMMATRIX* world) { worldMatrix = world; matricesDirty = true; }

    XMMATRIX GetLocalTransformationMatrix() const;
    XMMATRIX* GetWorldTransformationMatrix() const { return worldMatrix; }
    XMVECTOR getPosition() const { return position; }
    XMVECTOR getLookAt() const;
    XMVECTOR getUp() const;
    XMVECTOR getRight() const;
    XMMATRIX getRotation() const;
    XMMATRIX getTranslation() const;
    XMMATRIX getScaling() const;

    void Rotate(float fRotateX, float fRotateY, float fRotateZ, Space space = Space::Local);
    void Turn(float fRotateX, float fRotateY, float fRotateZ, Space space = Space::Local);
    void Position(float x, float y, float z);
    void Move(float x, float y, float z, Space space = Space::Local);
    void Scale(float x, float y, float z);
    void LookAt(const XMVECTOR& target, const XMVECTOR& upVector);
    float Distance(const XMVECTOR& target) const;

    // =========== NEUE FEATURES ===========

    // 1. QUATERNION-METHODEN
    XMVECTOR GetRotationQuaternion() const { return rotationQuat; }
    void SetRotationQuaternion(const XMVECTOR& quaternion);
    void RotateQuaternion(const XMVECTOR& quaternion, Space space = Space::Local);

    // 2. EULER-WINKEL GETTER
    float GetPitch() const;     // In Grad
    float GetYaw() const;       // In Grad
    float GetRoll() const;      // In Grad

    // 3. SKALIERUNG GETTER/SETTER
    XMVECTOR GetScaleVector() const { return scale; }
    void SetScale(float x, float y, float z);
    void SetScale(float uniformScale);

    // 4. TRANSFORM OPERATIONEN
    void Translate(const XMVECTOR& translation, Space space = Space::Local);
    void SetPosition(const XMVECTOR& pos);

    // 5. INTERPOLATION
    void Lerp(const Transform& target, float t);
    void Slerp(const Transform& target, float t);

    // 6. HELPER
    bool HasChanged() const { return matricesDirty; }
    void SetChanged(bool changed = true) { matricesDirty = changed; if (changed) vectorsDirty = true; }
    XMMATRIX GetWorldMatrix() const;

    // 7. TRANSFORM COMBINATIONS
    Transform Combine(const Transform& other) const;
    Transform Inverse() const;

    // 8. CONSTANT BUFFER DATA
    struct TransformData {
        DirectX::XMFLOAT4X4 worldMatrix;
        DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;
    };
    TransformData GetTransformData() const;

    // Legacy-Methoden
    float getRoll(const XMMATRIX* XMMatrix_p_Rotation) const;
    float getYaw(const XMMATRIX* XMMatrix_p_Rotation) const;
    float getPitch(const XMMATRIX* XMMatrix_p_Rotation) const;
};