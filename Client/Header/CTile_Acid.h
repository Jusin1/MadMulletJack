#pragma once
#include "CTileBase.h"
class CTile_Acid : public CTileBase
{
protected:
	explicit CTile_Acid(LPDIRECT3DDEVICE9 pGraphicDevice);
	explicit CTile_Acid(const CTile_Acid &rhs);
	virtual ~CTile_Acid();

	virtual void Free();
public:
	static CTile_Acid *Create(LPDIRECT3DDEVICE9 pGraphicDevice);
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
};

