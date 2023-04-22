#pragma once
#include "stdafx.h"

float wrapAngle(float angle) 
{
    if (angle > 180) {
        angle -= 360;
    }
    else if (angle < -180) {
        angle += 360;
    }
    return angle;
}

XMFLOAT3 rotateTowards(const TransformComponent& transform, const XMFLOAT3& targetAngles, float rotateSpeed, float deltaTime) 
{
    // Get the current rotation in euler angles
    XMFLOAT3 currentAngles = { transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z };

    // Calculate the rotation needed to reach the target angles
    XMFLOAT3 deltaAngles = {targetAngles.x - currentAngles.x,
       targetAngles.y - currentAngles.y, 
        targetAngles.z - currentAngles.z};

    // Ensure the angles wrap around correctly
    deltaAngles.x = wrapAngle(deltaAngles.x);
    deltaAngles.y = wrapAngle(deltaAngles.y);
    deltaAngles.z = wrapAngle(deltaAngles.z);

    // Calculate the rotation step for this frame
    XMFLOAT3 stepAngles = { deltaAngles.x * (rotateSpeed * deltaTime),
        deltaAngles.y * (rotateSpeed * deltaTime),
        deltaAngles.z * (rotateSpeed * deltaTime) };

    // Apply the rotation step to the game object
    return stepAngles;
}
