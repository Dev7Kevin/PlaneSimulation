// ShockWave.h: interface for the CShockWave class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#pragma once
 

#include "Unit.h"

//////////////////////////////////////////////////////////////////////////////
//
// class CShockWave 
//
// '미사일' 등의 '폭파時' 충격파 효과를 출력한다.
//  2004.06.30. mad_dog
//
class CShockWave : public CUnit  
{	
 	float	m_fSWRadius;			//충격파 현재 반경.
	//float	m_fSWMaxRadius;			//충격파 최고 반경.
	float	m_fSWThick;				//충격파 '링'의 두께.
	COLOR	m_Color;				//충격파 '색상'

	typedef struct {				//충격파용 정점 구조체.
		VECTOR3 vPos;
	 	FLOAT	tu, tv;
	}VERTEX_SHOCKWAVE;

	enum {
	  FVF_SHOCKWAVE_ = D3DFVF_XYZ | D3DFVF_TEX1,
 	};
	

 	LPVERTEXBUFFER	m_pVB;			//충격파 '링' 구성용 정점 버퍼..
	int				m_VertexCnt;	//총 정점의 개수.

	//페이드 아웃..
	BOOL	m_bFadeOut;
	FLOAT	m_fAlpha;
		 
public:
	CShockWave(
		 LPDEVICE	pDev,			//디바이스 포인터.
 		 float		fSWRadius,		//충격파 시작 반경. 
 		 float		fSWThick,		//충격파  두께.
		 COLOR		Color,			//연기 색상. 
 		 LPANIINFO	pBoomAniInfo,	//폭파 애니메이션 정보 구조체.	04.06.23+ 
									//: NULL 이면 무시된다.
 		 VECTOR3	vPos			//'월드 위치'-> 충격파의 중심좌표.

 		 //이하 -> 필요 없음.
		 //VECTOR3 vScale = VECTOR3(1, 1, 1),			//스케일. -> 스케일값...
  		 //VECTOR3 movespeed = VECTOR3(0, 0, 0),		//이동속도.
		 //VECTOR3 anglespeed = VECTOR3(0, 0, 0)		//회전 속도.
		 //CB3Mesh* pMesh,								//메쉬 포인터.
 		);


	virtual ~CShockWave();

	// 기본 메소드.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);		
	virtual void Release();

	//추가 메소드.
	BOOL Init();

	void SetAlpha(float alpha){ m_fAlpha = alpha; }
	void SetFadeOutState(BOOL bSet) { m_bFadeOut = bSet; }

	const float& GetSWRadius(){ return m_fSWRadius;}
	const float& GetAlpha(){ return m_fAlpha; }
	const BOOL&  GetFadeOutState() const { return m_bFadeOut; } 
	
	
	

};


/***************** end of file "ShockWave.h" ****************************/ 
