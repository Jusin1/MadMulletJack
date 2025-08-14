#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CVIBuffer_Cube_Color;
}

class CPlacementObject : public CGameObject
{
private:
	explicit CPlacementObject(LPDIRECT3DDEVICE9 pGraphicDevice);
	explicit CPlacementObject(const CPlacementObject &rhs);
	virtual ~CPlacementObject();

	virtual void Free();
public:
	static CPlacementObject *Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject *Clone(void *pArg = nullptr) override;
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int Update_GameObject(const _float &fTimeDelta) override;
	virtual void LateUpdate_GameObject(const _float &fTimeDelta) override;
	virtual void Render_GameObject() override;

	virtual _bool Picking(_vec3 *PickingPoint) override;
	virtual void PickingTrue() override;
	virtual void ExportData(void *pData) override;
private:
	HRESULT			Set_Component(void *pArg);
public:
	Engine::CVIBuffer_Cube_Color *GetBuffer() { return m_pBuffer; }
private:
	Engine::CVIBuffer_Cube_Color *m_pBuffer;
};

