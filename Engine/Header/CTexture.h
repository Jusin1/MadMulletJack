#pragma once

#include "CComponent.h"

BEGIN(Engine)

// 텍스쳐 로딩 및 관리
class ENGINE_DLL CTexture : public CComponent
{
public:
	typedef struct tagTextureInfo // 텍스처 애니메이션 프레임 정보
	{
		int m_iStart = 0; // 시작 프레임
		int m_iCurrentTex = 0; // 현재 프레임
		int m_iEndTex = 0; // 마지막 프레임
		_float m_fSpeed = 1; // 프레임 속도
		_bool m_bLoop = true; // 애니메이션 반복 여부
	}TEXINFO;
protected:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture();

public:
	HRESULT	Ready_Texture(TEXTUREID eType, const _tchar* pPath, const _uint& iCnt); // 텍스쳐 로딩
	HRESULT Initialize(void* pArg)override; // 초기화 함수(복사 시 호출)
	void	Set_Texture(const _uint& iIndex = 0); // 텍스처 바인딩(렌더링 시)
	IDirect3DBaseTexture9 *Get_Texture(const _uint &iIndex = 0) { return m_vecTexture[iIndex]; }

	bool Is_AnimFinished() const { return m_TextureInfo.m_iCurrentTex >= m_TextureInfo.m_iEndTex; }

public:
	void MoveFrame(); // 애니메이션 프레임 이동
	void Set_Frame(int iStart, int iEnd, int iSpeed, _bool bLoop); // 애니메이션 프레임 설정
	void Set_Zero_Frame() { m_TextureInfo.m_iCurrentTex = 0; } // 현재 프레임을 0 으로 초기화
	TEXINFO& Get_Frame() { return m_TextureInfo; } // 프레임 정보 가져오기
	void Stop_Anim() { m_bStopAnim = true; } // 애니메이션 정지
	void Resume_Anim() { m_bStopAnim = false; } // 다시재생
	void SetOriginCompName(std::wstring _wstr) { m_OriginComponentName = _wstr; }
	std::wstring GetOriginCompName() { return m_OriginComponentName; }
private:
	_uint								m_iNumTextures = 0; // 텍스쳐 수
	vector<IDirect3DBaseTexture9*>		m_vecTexture; // 텍스쳐 컨테이너
	TEXINFO								m_TextureInfo; // 프레임 정보
	_float m_fTimeAcc = 0.0f; // 시간 누적용
	bool								m_bStopAnim = false;
	std::wstring						m_OriginComponentName; // 원본 컴포넌트 이름 (파싱용)

public:
	virtual CComponent* Clone(void* pArg);
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		TEXTUREID eType,
		const _tchar* pPath, 
		const _uint& iCnt = 1);

private:
	virtual void	Free();

};
END