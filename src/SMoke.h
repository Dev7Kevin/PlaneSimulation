// SMoke.h: interface for the CSmoke class.
//
//
////////////////////////////////////////////////////////////////////////////// 
  
#pragma once
 

#include "Unit.h"
#include "Camera.h"

//////////////////////////////////////////////////////////////////////////////
//
// class CSmoke : �̻����� '����'� ���� Ŭ����.
//
class CSmoke : public CUnit  
{
	//��������Ʈ ������...
	B3SPRITE m_Spr;

	//'����' ����.
	COLOR	m_Color;

	//�������� ���� ī�޶� ������ �ʿ�.
	CCamera** m_ppCamera;

	//'���� �ð�' : ���ʰ� �ϴÿ� �� ������...
	int		m_ShowTime;
	//float	m_ElapsedTime;		//��� �ð�.(������ ���� ��������..)
	int		m_NowTime;
	int		m_OldTime;

	//'���̵� �ƿ�  ����. : ���� �������....  
	BOOL		m_bFadeOut;			//���̵� �ƿ� ���� ����.
	int			m_FadeTime;			//���̵� �ƿ� ���� �ð�.
	float		m_fAlpha;			//���̵� �ƿ��� ���� ���İ�.
	
	
public:

	CSmoke(
		 LPDEVICE pDev,				//����̽� ������.
		 CB3Mesh* pMesh,			//�޽� ������.

		 B3SPRITE* pSpr,				//��������Ʈ ������.
		 CCamera** ppCamera,		//ī�޶� ����������. 
		 int		ShowTime,		//���� �ð�.
		 int		FadeTime,		//���̵� �ð�.
		 COLOR		Color,			//���� ����.
		 
		 VECTOR3 vPos,				//'���� ��ġ'
		 VECTOR3 vScale = VECTOR3(1, 1, 1),			//������.
		 VECTOR3 vRot = VECTOR3(0, 0, 0),			//'ȸ��'
  		 VECTOR3 vMoveVelo = VECTOR3(0, 0, 0),		//�̵��ӵ�.
		 VECTOR3 vRotVelo = VECTOR3(0, 0, 0)		//ȸ�� �ӵ�.
		 );

	virtual ~CSmoke();


	// �⺻ �޼ҵ�.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();


};


/****************** end of file "Smoke.h" ********************************/ 
