// Enemy.h: interface for the CEnemy class.
//
//
////////////////////////////////////////////////////////////////////////////// 
 
#pragma once
 
#include "Plane.h"

//
// class CEnemy : �� ����� Ŭ���� 
//
class CEnemy : public CPlane  
{
protected:
	
	static int m_EnemyCnt;	//���� ī����.

	//�߰� ������....
	int	m_SNum;				//���� �ø��� �ѹ�.


	// ���� �⵿ ���� �߰� ���.
	int		m_ThrottleState;		//������ ���� �߷� ����..
	int		m_ThrottleStateOldTime;

	int		m_PitchRollState;		//��ġ, �� ����..
	int		m_PitchRollStateOldTime;



	//'Lock-On' üũ ����. 2004.11.15.
	BOOL	m_bLockON;							


public:
	CEnemy ( 
		 LPDEVICE	pDev,			//����̽� ������.
		 CB3Mesh*	pMesh,			//�޽� ������.
 		 UINT		hp,				//���� ü��.
		 float		fRadius,		//�浹�� �ݰ�.
		 LPANIINFO	pBoomAniInfo,	//���� �ִϸ��̼� ���� ����ü.04.06.23+ 
									//NULL �̸� ���õȴ�.
		 VECTOR3 vPos,				//���� ��ġ.
		 VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f), 
		 VECTOR3 vScale = VECTOR3(1.0f, 1.0f, 1.0f),
 		 VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
		 VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)
		 );

	virtual ~CEnemy();

	//�⺻ �޼ҵ�.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();

	//�߰� �޼ҵ�.
	const int& GetEnemyCnt() const { return m_EnemyCnt; }
	 

	//2004.11.15.
	const BOOL& IsLockON() const { return m_bLockON; }				
	void SetLockON(BOOL bLockOn = YES_) { m_bLockON = bLockOn; }		

};


/******************** end of file "Enemy.h" ***************************/
 
