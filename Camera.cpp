#include "Camera.h"

Camera::Camera()
{
	UpdateViewDir();
}

DirectX::SimpleMath::Matrix Camera::GetViewRow() const
{
	return DirectX::SimpleMath::Matrix::CreateTranslation(-mPosition)
		* DirectX::SimpleMath::Matrix::CreateRotationY(-mYaw)
		* DirectX::SimpleMath::Matrix::CreateRotationX(-mPitch); // mPitch가 양수이면 고개를 드는 방향
}

DirectX::SimpleMath::Matrix Camera::GetProjRow() const
{
	return DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(mProjFovAngleY),
		mAspect, mNearZ, mFarZ);
}

DirectX::SimpleMath::Vector3 Camera::GetEyePos() const
{
	return mPosition;
}

void Camera::UpdateViewDir()
{
	mViewDir = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f),
		DirectX::SimpleMath::Matrix::CreateRotationY(this->mYaw));
	mRightDir = mUpDir.Cross(mViewDir);
}
