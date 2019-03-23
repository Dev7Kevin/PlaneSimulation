// RailGun.h: interface for the CRailGun class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#pragma once
 

#include "Bullet.h"
#include "Camera.h"				//'������'�� ���� ī�޶� ������ �ʿ�.


///////////////////////////////////////////////////////////////////////////////
//
// class CRailGun : ���ϰ� �Ѿ� Ŭ����  
//
//////////////////////////////////////////////////////////////////////////////
class CRailGun : public CBullet
{
//------------------------------------------------------------------------	
//#define RAILGUN_TEST_			//�Ѿ� �׽�Ʈ��.
#ifdef RAILGUN_TEST_		 
	static int	m_RailGunSNCnt;	//�� �߻�� ī��Ʈ ����. (�߻���� ���� �Ȱ� ����)
	int			m_RailGunSN;	//�߻�� ���ϰ� ��ȣ. : ���� ���߿�.
#endif
//------------------------------------------------------------------------	

	//���ϰ� '����'�� : �߻�� ���ϰ� ��.
	static int m_RailGunCnt;	 

 
	//
	// ���ϰ� ���� ������  
	//
	// * ������ ���Ǵ� m_pSpr / m_Color / m_ppCamera �� CMissile �� ó���� 
	// * �����ϱ� ���� ���� Ŭ������ CBullet ���� �ø�. 
	// * �����ڴ� ���� ����.
	//   

	
public:
 
	//������.
	CRailGun(
		LPDEVICE pDev,
		CB3Mesh* pMesh,			//�Ѿ˿� �޽� ������.
		SPRDATA* pSpr,			//�Ѿ˿� ��Ʈ����Ʈ ������.. ���߿� �ϳ��� �־�� �Ѵ�.

		CCamera** ppCam,		//ī�޶� ���� ������. 2004.06.19. ����.

		UINT	 Dmg,			//�Ѿ� ������.
		COLOR	 Color,			//�Ѿ� �÷�.
		float	fFireVelo,		//�ѱ� �߻� ���ӵ�
		VECTOR3 vFireDir,		//�߻� ����.
		float	fMaxRange,		//�ִ� ��Ÿ�...

		float	 fRadius,			//�浹�� �ݰ�.

		VECTOR3 vPos,			//�Ѿ� �߻� ��ġ.
		VECTOR3 vScale,			//�Ѿ� ũ��. CBullet �� ������ �Ķ���� ��������
		VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f), 
		VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)
		 );

	virtual ~CRailGun();

	// �⺻ �޼ҵ�.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();

	//���ϰ� �߻�� ���� ���.
	static int& GetRailGunCnt() { return m_RailGunCnt; }

};




/********************** end of file "RailGun.h" *************************/ 
