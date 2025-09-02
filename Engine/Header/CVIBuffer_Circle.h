#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

struct CircleBufferData
{
	_float fRadius{ 1.f };
	_uint iSegments{ 32 };
	D3DXCOLOR Tint = D3DXCOLOR{ 1.0f, 0.f, 0.f, 0.75f };
};

class ENGINE_DLL CVIBuffer_Circle : public CVIBuffer
{
private:
	explicit CVIBuffer_Circle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer_Circle(const CVIBuffer_Circle &rhs);
	virtual ~CVIBuffer_Circle();

	virtual void Free() override;
public:
	virtual CComponent *Clone(void *pArg) override;
	static CVIBuffer_Circle *Create(LPDIRECT3DDEVICE9 pGraphicDev);

	virtual HRESULT Ready_Buffer() override;
	virtual HRESULT Initialize(void *pArg) override;

	virtual void Render_Buffer() override;
private:
	HRESULT Recreate_Buffers();
	HRESULT Fill_Positions_ToVB();
	HRESULT Fill_Indice_ToIB();
	HRESULT ApplyWhiteToVB();
public: 
	void Set_Tint(const D3DXCOLOR &_c);
	void Set_Radius(_float fRadius);
	void Set_Segments(_uint _i);
private:
	_uint m_iSegments{ 32 };
	_float m_fRadius{ 1.f };
	D3DXCOLOR m_Tint{ 1.f, 0.f, 0.f, 0.75f };
	DWORD m_dwTint{ 0xD9FF0000 };
	DWORD m_prevValue{ 0 };
};

END