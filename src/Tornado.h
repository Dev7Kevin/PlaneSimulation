// Tornado.h: interface for the CTornado class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#pragma once
 
  
#include "RailGun.h"

///////////////////////////////////////////////////////////////////////////////
//
// class CTornado : ����̵� �Ѿ� Ŭ���� 
//
//////////////////////////////////////////////////////////////////////////////
class CTornado : public CRailGun
{ 
protected:

	// ������ �߻� �ڵ�.
	VECTOR3		m_vSpiralPos;
	float		m_fSpiralVal;		
	
	MATRIX		m_mFireRot;		//������ �߻�� ȸ�����.

public:
 
	//������.
	CTornado(
		LPDEVICE pDev,
		CB3Mesh* pMesh,			//�Ѿ˿� �޽� ������.
		SPRDATA* pSpr,			//�Ѿ˿� ��Ʈ����Ʈ ������.. ���߿� �ϳ��� �־�� �Ѵ�.

		CCamera** ppCam,		//ī�޶� ���� ������. 2004.06.19. ����.

		UINT	 Dmg,			//�Ѿ� ������.
		COLOR	 Color,			//�Ѿ� �÷�.
		float	fFireVelo,		//�ѱ� �߻� ���ӵ�
		VECTOR3 vFireDir,		//�߻� ����. 
		MATRIX	mFireRot,		//�߻��, ������ȸ�����...
		float	fMaxRange,		//�ִ� ��Ÿ�...

		float	 fRadius,		//�浹�� �ݰ�.

		VECTOR3 vPos,			//�Ѿ� �߻� ��ġ.
		VECTOR3 vScale,			//�Ѿ� ũ��. CBullet �� ������ �Ķ���� ��������
		VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f),  
		VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)
		 );

	virtual ~CTornado();

	// �⺻ �޼ҵ�.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();

 

};




/********************** end of file "Tornado.h" *************************/ 
