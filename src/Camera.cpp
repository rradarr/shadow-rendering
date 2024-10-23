#include "stdafx.hpp"
#include "Camera.hpp"


void Camera::InitCamera(DirectX::XMFLOAT4 cameraPosition, DirectX::XMFLOAT4 cameraTarget, float frameAspectRatio)
{
    // build projection and view matrix
    DirectX::XMMATRIX tmpMat = DirectX::XMMatrixPerspectiveFovLH(45.0f * (3.14f / 180.0f), frameAspectRatio, 0.1f, 20.f);
    XMStoreFloat4x4(&projMat, tmpMat);

    // set starting camera state
    position = cameraPosition;
    target = cameraTarget;
    up = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

    // build view matrix
    DirectX::XMVECTOR cPos = DirectX::XMLoadFloat4(&position);
    DirectX::XMVECTOR cTarg = DirectX::XMLoadFloat4(&target);
    DirectX::XMVECTOR cUp = DirectX::XMLoadFloat4(&up);
    tmpMat = DirectX::XMMatrixLookAtLH(cPos, cTarg, cUp);
    XMStoreFloat4x4(&viewMat, tmpMat);

    localUp = DirectX::XMLoadFloat4(&up);
    localFront = DirectX::XMVector4Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&target), DirectX::XMLoadFloat4(&position)));
    localRight = DirectX::XMVector3Cross(localUp, localFront);

    globalUp = {0, 1, 0};
    globalRight = {1, 0, 0};
    globalFront = {0, 0, 1};
    camTarget = DirectX::XMLoadFloat4(&target);
    camPosition = DirectX::XMLoadFloat4(&position);
}

void Camera::UpdateCamera(DirectX::XMFLOAT2 lookRotation, DirectX::XMFLOAT3 movement, double deltaTime)
{
    const float fDeltaTime = static_cast<float>(deltaTime);
    const float lookSpeedScale = 0.6f * fDeltaTime;
    const float moveSpeedScale = 1.8f * fDeltaTime;

    DirectX::XMMATRIX camRotationMatrix;
    float camYaw = lookRotation.x * lookSpeedScale;
    float camPitch = lookRotation.y * lookSpeedScale;
    camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);

    DirectX::XMVECTOR rotAngles = DirectX::XMVECTOR{ camPitch , camYaw, 0 };

    DirectX::XMMATRIX localXrot = DirectX::XMMatrixRotationAxis(localRight, camPitch);
    DirectX::XMMATRIX localYrot = DirectX::XMMatrixRotationAxis(localUp, camYaw);
    //camTarget = DirectX::XMVector3TransformCoord(localFront, camRotationMatrix);
    //camTarget = DirectX::XMVector3Normalize(camTarget);

    rotMat = localXrot * localYrot;

    localRight = DirectX::XMVector3TransformNormal(localRight, localXrot);
    localUp = DirectX::XMVector3TransformNormal(localUp, localXrot);
    localFront = DirectX::XMVector3TransformNormal(localFront, localXrot);

    localRight = DirectX::XMVector3TransformNormal(localRight, localYrot);
    localUp = DirectX::XMVector3TransformNormal(localUp, localYrot);
    localFront = DirectX::XMVector3TransformNormal(localFront, localYrot);

    localRight = DirectX::XMVector3Normalize(localRight);
    localUp = DirectX::XMVector3Normalize(localUp);
    localFront = DirectX::XMVector3Normalize(localFront);
    /*localRight = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(localRight, camRotationMatrix));
    localFront = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(localFront, camRotationMatrix));
    localUp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(localFront, localRight));*/

    camTarget = localFront;

    camPosition = DirectX::XMVectorAdd(camPosition, DirectX::XMVectorScale(localRight, movement.x * moveSpeedScale));
    camPosition = DirectX::XMVectorAdd(camPosition, DirectX::XMVectorScale(localUp, movement.y * moveSpeedScale));
    camPosition = DirectX::XMVectorAdd(camPosition, DirectX::XMVectorScale(localFront, movement.z * moveSpeedScale));

    camTarget = DirectX::XMVectorAdd(camPosition, camTarget);

    DirectX::XMStoreFloat4x4(&viewMat, DirectX::XMMatrixLookAtLH(camPosition, camTarget, localUp));
}

void Camera::GetPostProjectionPositions(std::vector<DirectX::XMFLOAT4> &positions, bool clip)
{
    // Prepare the camera transform.
    DirectX::XMMATRIX projectionMat = DirectX::XMLoadFloat4x4(&this->projMat);
    DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&this->viewMat);
    DirectX::XMMATRIX VPMat = DirectX::XMMatrixMultiply(viewMat, projectionMat);

    for(auto &position : positions) {
        // Transform from world into camera NDC.
        DirectX::XMVECTOR point = DirectX::XMLoadFloat4(&position);
        point = DirectX::XMVector4Transform(point, VPMat);
        point = DirectX::XMVectorScale(point, 1.f/DirectX::XMVectorGetW(point));

        if(clip) {
            point = ClipXYToNDCCube(point);
        }
        
        DirectX::XMStoreFloat4(&position, point);
    }
}

void Camera::GetPostViewPositions(std::vector<DirectX::XMFLOAT4> &positions)
{
    // Prepare the camera transform.
    DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&this->viewMat);

    std::vector<DirectX::XMFLOAT4> viewSpacePositions;
    viewSpacePositions.resize(positions.size());
    DirectX::XMVector4TransformStream(
        viewSpacePositions.data(),
        sizeof(DirectX::XMFLOAT4),
        positions.data(),
        sizeof(DirectX::XMFLOAT4),
        positions.size(),
        viewMat
    );

    positions = viewSpacePositions;
}

DirectX::XMVECTOR Camera::ClipXYToNDCCube(DirectX::XMVECTOR position)
{
    // NOTE: WARNING: This does not perform correct clipping, as it clips line
    // a-b against plane p as a-(0) against p!!!

    // Clip the frustum points to [-1:1] range.
    // Get rid of unnecesary zw dimensions for clipping plane selection.
    DirectX::XMVECTOR point2D = DirectX::XMVectorMultiply(position, DirectX::XMVectorSet(1, 1, 0, 0));

    // Check if clipping is needed.
    uint32_t comparisonValue = 0;
    DirectX::XMVECTOR bounds = DirectX::XMVectorSet(1, 1, 1, 1);
    DirectX::XMVectorInBoundsR(&comparisonValue, point2D, bounds);
    if(!DirectX::XMComparisonAnyOutOfBounds(comparisonValue))
    {
        // X and Y in [-1;1] bounds, no need to clip.
        return position;
    }
    // X or Y are out of [-1;1] bounds, need to clip.
    DirectX::XMVECTOR pointDirection = DirectX::XMVector4Normalize(point2D);

    // Decide which side to clip with:
    DirectX::XMVECTOR clippingPlane;
    float rad = DirectX::XMVectorGetX(DirectX::XMVector2AngleBetweenNormals(DirectX::XMVectorSet(0, 1, 0, 0), pointDirection));
    if(rad <= DirectX::XM_PI * 0.25f) {
        // Clip against top.
        clippingPlane = DirectX::XMVectorSet(0, 1, 0, -1); // The d param seems to need to be negated than what intuition says? (goes against the normal)
    }
    else if (rad > DirectX::XM_PI * 0.75f) {
        // Clip against bottom.
        clippingPlane = DirectX::XMVectorSet(0, 1, 0, 1);
    }
    else {
        // Decide if clip against right or left.
        rad = DirectX::XMVectorGetX(DirectX::XMVector2AngleBetweenNormals(DirectX::XMVectorSet(1, 0, 0, 0), pointDirection));
        if(rad <= DirectX::XM_PI * 0.25f) {
            // Clip against right.
            clippingPlane = DirectX::XMVectorSet(1, 0, 0, -1);
        }
        else { //if (rad > DirectX::XM_PI * 0.75f) { // Well we don't this last case.
            // Clip against left.
            clippingPlane = DirectX::XMVectorSet(1, 0, 0, 1);
        }
    }

    // Perform the clipping.
    DirectX::XMVECTOR clippedPoint = DirectX::XMPlaneIntersectLine(clippingPlane, DirectX::XMVectorSet(0, 0, 0, 1), position);

    return clippedPoint;
}
