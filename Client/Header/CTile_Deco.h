#pragma once
#include "CTileBase.h"
class CTile_Deco : public CTileBase
{
protected:
	explicit CTile_Deco(LPDIRECT3DDEVICE9 pGraphicDevice);
	explicit CTile_Deco(const CTile_Deco &rhs);
	virtual ~CTile_Deco();

	virtual void Free();
public:
	static CTile_Deco *Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int Update_GameObject(const _float &fTimeDelta) override;
	virtual void LateUpdate_GameObject(const _float &fTimeDelta) override;
	virtual void Render_GameObject() override;

	void SetTextureIndex(int _i) { m_iTextureIndex = _i; }
private:
	virtual CGameObject *Clone(void *pArg = nullptr) override;
	HRESULT			Set_Component(void *pArg);
private:
	int m_iTextureIndex{ 0 };
};

