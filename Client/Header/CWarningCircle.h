#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CVIBuffer_Circle;
}

class CWarningCircle : public CGameObject
{
private:
	explicit CWarningCircle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWarningCircle(const CWarningCircle &rhs);
	virtual ~CWarningCircle();

	virtual void Free() override;
public:
	virtual CGameObject *Clone(void *pArg) override;
	static CWarningCircle *Create(LPDIRECT3DDEVICE9 pGraphicDev);

	virtual HRESULT Ready_GameObject();
	virtual HRESULT Initialize(void *pArg);
	virtual _int    Update_GameObject(const _float &fTimeDelta);
	virtual void    LateUpdate_GameObject(const _float &fTimeDelta);
	virtual void    Render_GameObject();

	virtual HRESULT Spawn_Pooling(void *pArg = nullptr) override;
	virtual HRESULT Despawn_Pooling() override;
private:
	HRESULT Set_Component();
	void Setup_OnGround();
	void RenderStates_Begin();
	void RenderStages_End();
private:
	CVIBuffer_Circle *m_pBufferCom{ nullptr };
	_float m_fElapsed{ 0.f };
	const _float m_fGoalTime{ 0.75f };
	_float m_fStart_Radius{ 0.25f };
	const _float m_fEnd_Radius{ 1.5f };
	_float m_fYOffset{ 0.02f };

	DWORD m_prevTFactor{ 0 };
	DWORD m_prevZWrite{ 0 };
	DWORD m_prevAlphaBlend{ 0 };
	DWORD m_prevSrcBlend{ 0 };
	DWORD m_prevDstBlend{ 0 };
	DWORD m_prevCullMode{ 0 };
};