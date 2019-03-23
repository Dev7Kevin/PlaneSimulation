// SkyBox.h: interface for the CSkyBox class.
//
//
////////////////////////////////////////////////////////////////////////////// 
 
#pragma once
 
#include "Unit.h"
#include "Plane.h"
#include "Camera.h"
//--------------------------------------------------------------------------//
// 스카이 박스(Sky Box) 제작 방법.											//
//																			//
// < 초기 설정 >															//
// 1. 6개의 텍스쳐 로드														//
// 2. Wrapping Mode 를 Clamp 로 처리.  ( 혹은 텍스쳐 좌표값의 보정)			//
// 3. bilinear 옵션 활성화.													//
//																			//
// <렌더링시 작업 순서>														//
// 1. 카메라를 중심으로 배치												//
// 2. 카메라의 변환행렬 적용.												//
// 3. 현재 렌더링 상태값 저장.												//											
// 4. 조명,안개, 블렌딩 등의 불필요한 렌더링 옵션 비활성화.					//
// 5. 깊이 버퍼 쓰기 (Z-Buffer Write) 비활성화.								//
// 6. 깊이 테스트 (Z-Depth Test) 비활성화.									//
// 7. 각 면을 렌더링.														//
// 8. 이전 상태 복구.														//
//																			//
//--------------------------------------------------------------------------//
//
//////////////////////////////////////////////////////////////////////////////
//
// class CSkyBox : 스카이 박스 출력 클래스.
//				   2004.06.30. Mad Dog / mad_dog@hanmail.net
//
class CSkyBox : public CUnit 
{
public:
	enum //SKYBOXPLANE
	{
		SBF_FRONT_,
		SBF_RIGHT_,
		SBF_REAR_,
		SBF_LEFT_,
 		SBF_TOP_,
		SBF_BOTTOM_,

		SBF_MAX_,

		FVF_SKYBOX = D3DFVF_XYZ | D3DFVF_TEX1,		//@fix. 2009.06.29.
		//VTX_CNT_ = 6 * 2 * 3;						//36개.
	};


	struct SBOX_VTX
	{
		//D3DXVECTOR3	position;		//정점 구조체.
		FLOAT		x, y, z;
		FLOAT		u, v;			//텍스쳐 좌표. 
	};


protected:

	//면 6개의 정보.
	SPRDATA m_Spr[SBF_MAX_];			//'외부'에서 로드한다.
	//MATRIX  m_SprTM[SBF_MAX_];			//'스프라이트' 전체 변환 행렬 배열. 

	CPlane*		m_pHero;			//주인공 정보 포인터.
	CCamera**	m_ppCamera;			//카메라 포인터.


	//하늘상자 모델버퍼.
	LPDIRECT3DVERTEXBUFFER9	m_pVBSBox;		//정점 버퍼용 포인터.
	HRESULT _CreateVB();

  

protected: 
	//추가 메소드.
	int	 Init();
	int  DrawSB(int num);
	
public:
		 
	CSkyBox( 
		LPDEVICE	pDev,						//디바이스 포인터.
		CPlane*		pHero,						//주인공 정보 포인터.
		CCamera**	ppCamera,					//카메라 포인터.
		
		SPRDATA		pSpr[SBF_MAX_],						//스카이 박스용 텍스쳐 저장용 스프라이트  배열.
		VECTOR3		vPos = VECTOR3(0, 0, 0),			//박스 위치 : (월드 위치)
		VECTOR3		vScale = VECTOR3(512, 512, 512)		//박스 크기 : 약 512 정도.
  		);



	virtual ~CSkyBox();


	//기본 메소드.
	virtual int  Draw(float dTime = 0);		//렌더링 메소드.
 	virtual int  Move(float dTime = 0);		//스카이 박스의 '이동, 회전' 등의 처리
	virtual void Release();

};


/******************** end of file "SkyBox.h" ******************************/ 
