#pragma once
#include "CUI.h"

class CImageUI;
class CTalkUI :
    public CUI
{
protected:
	explicit CTalkUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTalkUI(const CTalkUI& rhs);
	virtual ~CTalkUI();

public: 
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int    Update_GameObject(const _float& fTimeDelta) override;
	virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual void    Render_GameObject() override;

public: // 대화 관리
	void LoadDialogues(const vector<wstring>& dialogues); // 대화 리스트 로드
	void NextDialogue();                                  // 다음 대사로 넘어감
	void Set_FramePos(float x, float y, float sizeX, float sizeY); // 프레임 위치/크기

	// 텍스트 위치/크기 조정
	void Set_TextPos(float offsetX, float offsetY) { m_vTextOffset = { offsetX, offsetY }; }
	void Set_TextScale(float scale) { m_fTextScale = scale; }

protected: 
	vector<wstring> m_vecDialogues;  // 대사 목록
	int             m_iCurrentIndex; // 현재 대사 인덱스
	wstring         m_CurrentText;   // 현재 출력할 대사
	wstring         m_DisplayText;   // 현재까지 출력된 글자

	CImageUI* m_pFrame;        // 대화창 프레임 UI

	float           m_fCharInterval; // 타이핑 효과: 글자 간격 시간
	float           m_fAccTime;      // 누적 시간
	bool            m_bTypingDone;   // 현재 대사 출력 완료 여부

	_vec2           m_vTextOffset;   // 텍스트 위치 오프셋
	float           m_fTextScale;    // 텍스트 크기 배율

	float           m_fFrameX;       // 프레임 X 좌표
	float           m_fFrameY;       // 프레임 Y 좌표
	float           m_fFrameSizeX;   // 프레임 가로 크기
	float           m_fFrameSizeY;   // 프레임 세로 크기

public: 
	static CTalkUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

