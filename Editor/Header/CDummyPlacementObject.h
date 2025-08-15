#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CVIBuffer_Cube_Color;
}

typedef struct tagPlacementObjectData
{
	MapEditorObjectCategory eCategory = MapEditorObjectCategory::NONE;
	_uint iType = 0;
	D3DXCOLOR dwColor;
	TRANSFORMDATA transform;
} PlacementObjectData;

class CDummyPlacementObject : public CGameObject
{
private:
	explicit CDummyPlacementObject(LPDIRECT3DDEVICE9 pGraphicDevice);
	explicit CDummyPlacementObject(const CDummyPlacementObject &rhs);
	virtual ~CDummyPlacementObject();

	virtual void Free();
public:
	static CDummyPlacementObject *Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject *Clone(void *pArg = nullptr) override;
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int Update_GameObject(const _float &fTimeDelta) override;
	virtual void LateUpdate_GameObject(const _float &fTimeDelta) override;
	virtual void Render_GameObject() override;

	virtual _bool Picking(_vec3 *PickingPoint) override;
	virtual void PickingTrue() override;
	virtual void ExportData(void *pData) override;

	void MakeObject(PlacementObjectData *pData);
	void MakeMonsterObject(MapEditorMonsterType _e, PlacementObjectData *pData);
	void MakeEnvObject(MapEditorEnvObjectType _e, PlacementObjectData *pData);
private:
	HRESULT			Set_Component(void *pArg);
	void			PosUpdate();
public:
	Engine::CVIBuffer_Cube_Color *GetBuffer() { return m_pBuffer; }
private:
	Engine::CVIBuffer_Cube_Color *m_pBuffer;
};

