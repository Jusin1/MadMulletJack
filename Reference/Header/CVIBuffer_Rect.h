#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect : public CVIBuffer
{
private:
	explicit CVIBuffer_Rect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer_Rect(const CVIBuffer_Rect& rhs);
	virtual ~CVIBuffer_Rect();

public:
	virtual HRESULT	Ready_Buffer();
	virtual HRESULT Initialize(void* pArg) override;
	
public:
	virtual _bool Picking(class CTransform* pTransform, _vec3* pOut = nullptr) override;

public:
	virtual CComponent* Clone(void* pArg) override;
	static CVIBuffer_Rect* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free();

private:
	_vec3 m_vVerticesLocal[4]; // 로컬 공간의 정점 위치 저장용
};

END