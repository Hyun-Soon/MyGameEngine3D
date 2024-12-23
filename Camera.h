#pragma once

#include <directxtk/SimpleMath.h>

class Camera
{
public:
	Camera();

	DirectX::SimpleMath::Matrix	 GetViewRow() const;
	DirectX::SimpleMath::Matrix	 GetProjRow() const;
	DirectX::SimpleMath::Vector3 GetEyePos() const;

	void Reset(DirectX::SimpleMath::Vector3 pos, float yaw, float pitch)
	{
		mPosition = pos;
		mYaw = yaw;
		mPitch = pitch;
		UpdateViewDir();
	}

	void UpdateViewDir();

private:
	DirectX::SimpleMath::Vector3	   mPosition = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
	DirectX::SimpleMath::Vector3	   mViewDir = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
	const DirectX::SimpleMath::Vector3 mUpDir = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	DirectX::SimpleMath::Vector3	   mRightDir = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

	// roll, pitch, yaw
	// https://en.wikipedia.org/wiki/Aircraft_principal_axes
	float mYaw = 0.0f, mPitch = 0.0f;

	float mSpeed = 3.0f; // 움직이는 속도

	// 프로젝션 옵션도 카메라 클래스로 이동
	float mProjFovAngleY = 90.0f * 0.5f; // Luna 교재 기본 설정
	float mNearZ = 0.01f;
	float mFarZ = 100.0f;
	float mAspect = 16.0f / 9.0f;
};