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
	explicit CGridPanel(const CGridPanel& rhs);
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

	HRESULT Change_Texture(_uint iSceneIdx, const _tchar *pPrototypeTag, void *pArg = nullptr);
	HRESULT Change_Buffer(_uint iSceneIdx, const _tchar *pPrototypeTag, void *pArg = nullptr);
private:
	HRESULT			Set_Component();
public:
	CVIBuffer_GridPanel *GetBuffer() { return m_pBuffer; }
	Engine::CRenderer *GetRenderer() { return m_pRenderer; }
	Engine::CTexture *GetTexture() { return m_pTexture; }
	Engine::CTransform *GetTransform() { return m_pTransform; }
private:
	CVIBuffer_GridPanel *m_pBuffer;
	Engine::CRenderer	*m_pRenderer;
	Engine::CTexture	*m_pTexture;
	Engine::CTransform	*m_pTransform;
};