// SkyBox.h: interface for the CSkyBox class.
//
//
////////////////////////////////////////////////////////////////////////////// 
 
#pragma once
 
#include "Unit.h"
#include "Plane.h"
#include "Camera.h"
//--------------------------------------------------------------------------//
// ��ī�� �ڽ�(Sky Box) ���� ���.											//
//																			//
// < �ʱ� ���� >															//
// 1. 6���� �ؽ��� �ε�														//
// 2. Wrapping Mode �� Clamp �� ó��.  ( Ȥ�� �ؽ��� ��ǥ���� ����)			//
// 3. bilinear �ɼ� Ȱ��ȭ.													//
//																			//
// <�������� �۾� ����>														//
// 1. ī�޶� �߽����� ��ġ												//
// 2. ī�޶��� ��ȯ��� ����.												//
// 3. ���� ������ ���°� ����.												//											
// 4. ����,�Ȱ�, ���� ���� ���ʿ��� ������ �ɼ� ��Ȱ��ȭ.					//
// 5. ���� ���� ���� (Z-Buffer Write) ��Ȱ��ȭ.								//
// 6. ���� �׽�Ʈ (Z-Depth Test) ��Ȱ��ȭ.									//
// 7. �� ���� ������.														//
// 8. ���� ���� ����.														//
//																			//
//--------------------------------------------------------------------------//
//
//////////////////////////////////////////////////////////////////////////////
//
// class CSkyBox : ��ī�� �ڽ� ��� Ŭ����.
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
		//VTX_CNT_ = 6 * 2 * 3;						//36��.
	};


	struct SBOX_VTX
	{
		//D3DXVECTOR3	position;		//���� ����ü.
		FLOAT		x, y, z;
		FLOAT		u, v;			//�ؽ��� ��ǥ. 
	};


protected:

	//�� 6���� ����.
	SPRDATA m_Spr[SBF_MAX_];			//'�ܺ�'���� �ε��Ѵ�.
	//MATRIX  m_SprTM[SBF_MAX_];			//'��������Ʈ' ��ü ��ȯ ��� �迭. 

	CPlane*		m_pHero;			//���ΰ� ���� ������.
	CCamera**	m_ppCamera;			//ī�޶� ������.


	//�ϴû��� �𵨹���.
	LPDIRECT3DVERTEXBUFFER9	m_pVBSBox;		//���� ���ۿ� ������.
	HRESULT _CreateVB();

  

protected: 
	//�߰� �޼ҵ�.
	int	 Init();
	int  DrawSB(int num);
	
public:
		 
	CSkyBox( 
		LPDEVICE	pDev,						//����̽� ������.
		CPlane*		pHero,						//���ΰ� ���� ������.
		CCamera**	ppCamera,					//ī�޶� ������.
		
		SPRDATA		pSpr[SBF_MAX_],						//��ī�� �ڽ��� �ؽ��� ����� ��������Ʈ  �迭.
		VECTOR3		vPos = VECTOR3(0, 0, 0),			//�ڽ� ��ġ : (���� ��ġ)
		VECTOR3		vScale = VECTOR3(512, 512, 512)		//�ڽ� ũ�� : �� 512 ����.
  		);



	virtual ~CSkyBox();


	//�⺻ �޼ҵ�.
	virtual int  Draw(float dTime = 0);		//������ �޼ҵ�.
 	virtual int  Move(float dTime = 0);		//��ī�� �ڽ��� '�̵�, ȸ��' ���� ó��
	virtual void Release();

};


/******************** end of file "SkyBox.h" ******************************/ 
