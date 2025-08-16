#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRenderer;
	class CTexture;
	class CTransform;
	class CVIBuffer_GridPanel;
}

class CGridPanel : public CGameObject
{
private:
	explicit CGridPanel(LPDIRECT3DDEVICE9 pGraphicDevice);
	explicit CGridPanel(const CGridPanel &rhs);
	virtual ~CGridPanel();

	virtual void Free();
public:
	static CGridPanel *Create(LPDIRECT3DDEVICE9 pGraphicDev);
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
public:
	CVIBuffer_GridPanel *GetBuffer() { return m_pBuffer; }
	Engine::CTexture *GetTexture() { return m_pTexture; }
	void SetType(WallType _e) { m_eType = _e; }
	WallType GetType() const { return m_eType; }
private:
	WallType m_eType;
	CVIBuffer_GridPanel *m_pBuffer;
	Engine::CTexture *m_pTexture;
};