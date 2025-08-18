#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CVIBuffer_Rect;
	class CTexture;
}

class CTileBase : public CGameObject
{
protected:
	explicit CTileBase(LPDIRECT3DDEVICE9 pGraphicDevice, TileType _e);
	explicit CTileBase(const CTileBase &rhs, TileType _e);
	virtual ~CTileBase();

	virtual void Free();
public:
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int Update_GameObject(const _float &fTimeDelta) override;
	virtual void LateUpdate_GameObject(const _float &fTimeDelta) override;
	virtual void Render_GameObject() override;

	const CVIBuffer_Rect *GetBuffer() const { m_pBuffer; }
	const CTexture *GetTexture() const { m_pTexture; }
	TileType GetType() const { return m_eType; }
	RENDERID GetRenderId() const { return m_eRenderID; }
private:
	virtual CGameObject *Clone(void *pArg = nullptr) override;
	HRESULT			Set_Component(void *pArg);
protected:
	TileType m_eType;
	CVIBuffer_Rect *m_pBuffer;
	// TODO - Render ³ª´©±â
	RENDERID m_eRenderID;
	Engine::CTexture *m_pTexture;
};

