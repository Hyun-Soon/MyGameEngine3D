#include "SkinnedMeshModel.h"

void SkinnedMeshModel::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
	// ConstBuffer 대신 StructuredBuffer 사용
	// context->VSSetConstantBuffers(3, 1, m_skinnedConsts.GetAddressOf());

	context->VSSetShaderResources(2, 1, mBoneTransforms.GetAddressOfSRV()); // 항상 slot index 주의

	// Skinned VS/PS는 GetPSO()를 통해서 지정되기 때문에
	// Model::Render(.)를 같이 사용 가능

	Model::Render(context);
}
