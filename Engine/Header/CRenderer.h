#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CGameObject.h"

BEGIN(Engine)

// 렌더링 처리하는 컴포넌트
class ENGINE_DLL CRenderer :    public CComponent
{

private:
	explicit CRenderer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRenderer();

public:
	virtual HRESULT Ready_Render(); 
	virtual HRESULT Initialize(void* pArg);
public:
	// 특정 렌더 그룹에 오브젝트 추가
	HRESULT		Add_RenderGroup(RENDERID eType, class CGameObject* pGameObject); 
	// 등록된 오브젝트들을 순서대로 렌더링
	HRESULT		Render_GameObject();

private:
	HRESULT		Render_Priority(); // 우선순위
	HRESULT		Render_NonAlpha(); // 불투명
	HRESULT		Render_Alpha(); // 반투명6
	HRESULT		Render_UI(); // UI 전용

private:
	// 렌더 그룹 오브젝트 리스트 배열
	list<class CGameObject* > m_RenderGroup[RENDER_END];
	typedef list<class CGameObject*>		GAMEOBJECTS;

public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGrahpicDev);
	virtual CComponent* Clone(void* pArg = nullptr)override;
	virtual void Free();
};
END
