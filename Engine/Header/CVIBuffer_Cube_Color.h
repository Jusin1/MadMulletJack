#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube_Color : public CVIBuffer
{
private:
	explicit CVIBuffer_Cube_Color(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer_Cube_Color(const CVIBuffer_Cube_Color &rhs);
	virtual ~CVIBuffer_Cube_Color();

public:
	virtual HRESULT	Ready_Buffer();
	virtual HRESULT Initialize(void *pArg) override;

public:
    virtual _bool Picking(class CTransform *pTransform, _vec3 *pOut = nullptr) override;
public:
	virtual CComponent *Clone(void *pArg) override;
	static CVIBuffer_Cube_Color *Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free();

private:
	D3DXCOLOR m_dwColor;
	_vec3 m_vVerticesLocal[8]; // 로컬 공간의 정점 위치 저장용
};

END
