#pragma once
#include "CGameObject.h"
namespace Engine
{
	class CVIBuffer_Rect;
	class CTexture;
}


class CDummyTile : public CGameObject
{
private:
	explicit CDummyTile(LPDIRECT3DDEVICE9 pGraphicDevice);
	explicit CDummyTile(const CDummyTile &rhs);
	virtual ~CDummyTile();

	virtual void Free();
public:
	static CDummyTile *Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject *Clone(void *pArg = nullptr) override;
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int Update_GameObject(const _float &fTimeDelta) override;
	virtual void LateUpdate_GameObject(const _float &fTimeDelta) override;
	virtual void Render_GameObject() override;

	virtual _bool Picking(_vec3 *PickingPoint) override;
	virtual void PickingTrue() override;
private:
	HRESULT			Set_Component(void *pArg);
	void PosUpdate();
	void KeyUpdate();
public:
	Engine::CVIBuffer_Rect *GetBuffer() { return m_pBuffer; }
	Engine::CTexture *GetTexture() { return m_pTexture; }
private:
	Engine::CVIBuffer_Rect *m_pBuffer;
	Engine::CTexture *m_pTexture;
};

