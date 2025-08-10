#pragma once
#include "CBase.h"
#include "CComponent.h"
#include "CTransform.h"
BEGIN(Engine)
class CRenderer;

class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject();

public:
	virtual			HRESULT		Ready_GameObject(); // 초기생성 호출
	virtual			HRESULT		Initialize(void* pArg); // 복사생성 호출
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
	void Set_Active(bool bActive) { m_bActive = bActive; }
	bool Is_Active() const { return m_bActive; }

protected:
	HRESULT	Set_Component();

public:
	virtual _bool Picking(_vec3* PickingPoint) { return false; }
	virtual void PickingTrue() {};
	virtual void HitAt(const _vec3& hitPosWorld) {} // 맞은 지점 전달
	virtual void ExportData(void *pData) {};
public:
	CTransform* GetTransform() const { return m_pTransformCom; }
protected:
	map<const _tchar*, CComponent*>			m_mapComponent;
	_vec3									m_vPosition;
	_float									m_fRadius;
	_bool									m_bDead;
	_matrix									m_CollisionMatrix; // 충돌 시 사용할 월드 행렬
	LPDIRECT3DDEVICE9						m_pGraphicDev;
	bool m_bActive;

protected:
	CTransform* m_pTransformCom;
	CRenderer* m_pRendererCom;

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