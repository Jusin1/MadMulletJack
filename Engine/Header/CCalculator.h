#pragma once
#include "CComponent.h"

BEGIN(Engine)

class CTerrainTex;
class CTransform;

class ENGINE_DLL CCalculator :   public CComponent
{
private:
	explicit CCalculator(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCalculator();

public:
	HRESULT			Ready_Calculator();
	_float			Compute_HeightOnTerrain(const _vec3* pPos,
											const _vec3* pTerrainVtxPos, 
											const _ulong& dwCntX,
											const _ulong& dwCntZ,
											const _ulong& dwVtxItv = VTXITV);

	_vec3			Picking_OnTerrain(HWND hWnd,
									  const CTerrainTex* pTerrainBufferCom,
									  const CTransform* pTerrainTransformCom);

private:
	LPDIRECT3DDEVICE9		m_pGraphicDev;

public:
	static CCalculator* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();
private:
	virtual void	Free();

};

END
