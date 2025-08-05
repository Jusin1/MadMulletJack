#pragma once
#include "CBase.h"
#include "CComponent.h"
#include "CTransform.h"
BEGIN(Engine)

class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject();

public:
	virtual			HRESULT		Ready_GameObject(); // 초기생성 호출
	virtual			HRESULT		Initialize(void* pArg) { return S_OK; } // 복사생성 호출
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject() {}

public:
	void Update_Position(_vec3 position) { m_vPosition = position; } // 위치 설정
	_vec3 Get_Position() { return m_vPosition; }
	_float Get_Radius() { return m_fRadius; } 
	_bool Get_Dead() { return m_bDead; }
	void Set_Dead(_bool bDead) { m_bDead = bDead; }
	void Set_Radius(_float fRadius) { m_fRadius = fRadius; }
public:
	CTransform* GetTransform() const { return m_pTransformCom; }
protected:
	map<const _tchar*, CComponent*>			m_mapComponent;
	_vec3									m_vPosition;
	_float									m_fRadius;
	_bool									m_bDead;
	_matrix									m_CollisionMatrix; // 충돌 시 사용할 월드 행렬
	LPDIRECT3DDEVICE9						m_pGraphicDev;


	// Trnasform Component
protected:
	CTransform* m_pTransformCom;

protected:
	// 컴포넌트 추가
	HRESULT Add_Components(const _tchar* pComponentTag, _uint iSceneIdx, const _tchar* pPrototypeTag, CComponent** ppOut, void* pArg = nullptr);
	// 컴포넌트 참조만 얻기
	HRESULT Change_Component(const _tchar* pComponentTag, CComponent** ppOut);
public:
	// 컴포넌트 찾기
	CComponent* Find_Component(const _tchar* pComponentTag);
public:
	virtual CGameObject* Clone(void* pArg = nullptr) PURE;
	virtual		void		Free();
};

END