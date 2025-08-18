#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CVIBuffer_Rect;
	class CTexture;
}

class CTile : public CGameObject
{
private:
	explicit CTile(LPDIRECT3DDEVICE9 pGraphicDevice);
	explicit CTile(const CTile &rhs);
	virtual ~CTile();

	virtual void Free();
public:
	static CTile *Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject *Clone(void *pArg = nullptr) override;
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int Update_GameObject(const _float &fTimeDelta) override;
	virtual void LateUpdate_GameObject(const _float &fTimeDelta) override;
	virtual void Render_GameObject() override;

	virtual _bool Picking(_vec3 *PickingPoint) override;
	virtual void PickingTrue() override;
	virtual void ExportData(void *pData) override;

	HRESULT Change_Texture(_uint iSceneIdx, const _tchar *pPrototypeTag, void *pArg = nullptr);
	HRESULT Change_Buffer(_uint iSceneIdx, const _tchar *pPrototypeTag, void *pArg = nullptr);

	_uint GetType() { return m_iType; }
	void SetType(_uint _i) { m_iType = _i; }
private:
	HRESULT			Set_Component(void *pArg);
public:
	Engine::CVIBuffer_Rect *GetBuffer() { return m_pBuffer; }
	Engine::CTexture *GetTexture() { return m_pTexture; }
private:
	_uint m_iType;
	Engine::CVIBuffer_Rect *m_pBuffer;
	Engine::CTexture *m_pTexture;
};