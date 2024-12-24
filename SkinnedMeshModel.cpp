#include "SkinnedMeshModel.h"

void SkinnedMeshModel::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
	// ConstBuffer ��� StructuredBuffer ���
	// context->VSSetConstantBuffers(3, 1, m_skinnedConsts.GetAddressOf());

	context->VSSetShaderResources(2, 1, mBoneTransforms.GetAddressOfSRV()); // �׻� slot index ����

	// Skinned VS/PS�� GetPSO()�� ���ؼ� �����Ǳ� ������
	// Model::Render(.)�� ���� ��� ����

	Model::Render(context);
}
