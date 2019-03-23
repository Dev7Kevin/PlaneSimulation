// Missile.h: interface for the CMissile class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#pragma once
 
//#include "Plane.h"
#include "Bullet.h"
#include "ArrayList.h"				//'����' ������ ���� ArrayList ���.
 



//////////////////////////////////////////////////////////////////////////////
//
// class CMissile : �̻��� Ŭ���� ( �ϴ� ������. None-Guided) 
//
// 1. ���ΰ� ��ü���� �����Ǿ� �����δ�.
// 2. �߻� ��ư�� ������ ���Ϸ����� 'Release' �ȴ�.
// 3. ����, ���Ϻν��� ��ȭ.  '����' ����. 
// 2. �߻� ���� �ڽ��� 'Seeker'�� ���� ��ǥ�� �����Ѵ�.(Fire & Forget) : => ����.
// 3. �����߿��� '����'�� �Ĺ����� '�վ��' 
// 4. �ִ� ��Ÿ��� �Ѿ�� '����' �Ѵ�.
//
//////////////////////////////////////////////////////////////////////////////

class CMissile : public CBullet  
{
protected:	
	//
	// �̻��� ����..
	//
	CPlane*		m_pPlane;			//�̻����� ������ '�����' ������. 
									//���⿡�� ��ȯ ��İ��� ���´�.: ex) ���ΰ�.

	//�ϵ�����Ʈ ��ġ.( �߻�Ǳ����� �̻����� ��ġ���� ����ϴµ� ����Ѵ�)
	//m_vPos - �̵��ϸ鼭 �����Ǹ� �߻� ���� �̰��� ���� ��ǥ�� ����Ѵ�.
	VECTOR3		m_vHardPt;			

 	BOOL		m_bIsFired;			//�߻� ����...�߻� �Ǿ��ٸ�..����/�̵� ���� ������ �ʿ�. 

	VECTOR3		m_vAstPos;


	// 
	// �̻��� ȭ�� ����.
	//
	VECTOR3		m_vSmokeScale;		//'ȭ��', '����' ������.
	VECTOR3		m_vSmokePos;		//'ȭ��', '����' ��ġ(�̻��� �Ĺ�:�ν��� ��ġ)
									//�Է��� �𵨰����� ��밪 :  ����� ���� ������ ��ġ��.
	VECTOR3		m_vSmokePosW;		//'����', '����' ������ġ.(���� ���� ���밪)
	
	//B3SPRITE*	m_pSprSmoke;		//'����'�� ���� 3D ��������Ʈ ������.

	//
	// '����' ���� �����͵�.
	//
	CArrayList* m_pSmokeList;		//���� ����Ʈ...
	int			m_OldTime;			//���� ��� �ð� ������.
	int			m_NowTime;
	B3SPRITE	m_sprSmoke;			//���� ��������Ʈ..


	//
	// �̻��� �߻� ���� : 2004.06.21+
	//
	BOOL	m_bIsReleased;		//'��ź', '�̻���'���� �ϵ�����Ʈ�� �и� �Ǿ��°�...����.
	BOOL	m_bIsReleasedAst;
	float	m_fRelaseLength;	//'������' ����. 
	VECTOR3 m_vReleasePos;		//������ ��ǥ.
	VECTOR3 m_vRelAstPos;
	VECTOR3 m_vReleaseTimePlaneDirVelo;		//������ �� ����� �ӵ� : ������ �ɶ��� ����� �ӵ��� 
											//�̻����� '������' �ӵ��� �ȴ�.
 	
	//
	// �̻��� �߰� ������ : 
	//
	VECTOR3 m_vUp;				//�̻����� ��� ����.
	VECTOR3 m_vNView;			//�̻����� �ü� ����.
	VECTOR3 m_vHorz;			//�̻����� ���� ����.

	float	m_fCurrVelo;		//�̻����� ���� ���ӵ� (����) 
 	

	//
	// ���� �ִϰ���... 2004.06.24.
	//
	//VECTOR3 m_vBoomSize;			//-> �ʿ� ����. 
	float	m_fBoomSphereRaidus;	//���� �� �ݰ�. 
	int	    m_BoomShowTime;			//���� ���� �ð�...

	float	m_fBoomSphereMaxRadius;	//���Ľ�  'ȭ����' �� �ݰ�. 
									// : �����..�̰��� ����� Ŀ���鼭..
									//   �ֺ��� ���ֱ��� �������� ������.

	BOOL	m_bFadeOut;				//���ı�..�������.
	float	m_fAlpha;				//���ı� ���İ�.

	// ����� ��ü 
	CShockWave*	m_pShockWave;	


	//�̻��� ���� ����.
	float	m_fRocketFuel;				//���� ����... �ٶ��� ����..�����Ѵ�.
	float	m_fMaxFlyVelo;				//�̻��� �ִ� ���� �ӵ�. : m_fMaxRange ��ſ����. 2004.11.12.
	float	m_fMaxAccelSpeed;			//�̻��� �ִ� ���ӵ�.

	//���� ����.
	CPlane*	m_pTarget;					//��ǥ ����.
	BOOL	m_bHommingON;				//���� �۵���...ON!



protected:

	void PutSmoke(float dTime);				//����ũ ��� : �̻����� �̵��� 
											//�����ð����� ���⸦ �����Ѵ�.

	void SetHardPointPos();						//�̻��� ��ġ ����.
	void SetSmokePos(VECTOR3* pvPos = NULL);	//�̻��� ȭ��& ���� ��ǥ ����.


public:

	CMissile(
		LPDEVICE	pDev,
		CB3Mesh*	pMesh,			//�̻��Ͽ� �޽� ������.

 		CCamera**	ppCam,			//ī�޶� ���� ������. 2004.06.19. ����.: �߻� ȭ�� ������.
  		UINT		Dmg,			//�̻��ϵ�����.
		
		float		fMaxFlyVelo,	//�̻����� �ִ� ���� �ӵ�.
		VECTOR3		vFlyDir,		//�̻����� ���� ����.	
 
		float		fMaxFuel,		//�ִ� ����. (��Ÿ�->���� �������� ��ȯ. 2004.11.12.)

 		CPlane*		pPlane,			//������ ����� ������.
		BOOL		bIsFired,		//�߻� �Ǿ���?

		B3SPRITE*	pSprFire,		//�̻��� ȭ���� ��Ʈ����Ʈ ������.. 
		VECTOR3		vSmokePos,		//'����', '����' ������ġ.(�𵨰��� ��밪)
 		VECTOR3		vSmokeScale,	//'ȭ��', '����' ������.
		COLOR		Color,			//ȭ�� �÷�.
		B3SPRITE*	psprSmoke,		//���� ��������Ʈ..
		
		CArrayList* pList,			//����ũ ����Ʈ..(������ ���⸦ ���. �ܺο��� �����ϰ� �ȴ�)

		float		fRadius,		//�浹�� �ݰ�.
		LPANIINFO	pBoomAniInfo,	//���� �ִϸ��̼� ���� ����ü.04.06.23+ 
									//NULL �̸� ���õȴ�.

		LPANIINFO	pShockWaveAniInfo,	//���� �ִϸ��̼� ���� ����ü. 
										//: NULL �̸� ���õȴ�.

		VECTOR3 vPos,			//�̻��Ϲ߻� ��ġ : �� ������� 
								//'�ϵ� ����Ʈ Hard Point' ��ġ�̴�.
								//(����� �𵨰��������� '��� ��ġ')

		VECTOR3 vScale,			//�̻���'ũ��'
		VECTOR3 vRot = VECTOR3(0.0f, 0.0f, 0.0f), 
 		VECTOR3 vMoveVelo = VECTOR3(0.0f, 0.0f, 0.0f),
 		VECTOR3 vRotVelo = VECTOR3(0.0f, 0.0f, 0.0f)


 		 );

	virtual ~CMissile();

protected:
	//----------------------------------------------------------------------------------//
	// �̻����� '�̵�'��  ũ�� 3�ܰ�� ������ ����.	  									//
	//																					//		
	// 1�ܰ� : ������� '�ϵ� ����Ʈ' �� �����Ǿ� ������ �Բ� �����̴� ��� : "On HP" //
	//			=> CMissile::MoveOnHardPoint()											//
	// 2�ܰ� : �߻��ư�� ���� '�ϵ� ����Ʈ'���� '�������� �ܰ�' : "RELEASE"			//
	//			=> CMissile::MoveReleaseTime()											//	
	// 3�ܰ� : ������ ��, '�ν���'��  ��ȭ�� ���� '����' �ϴ� �ܰ� : "FLY"				//	
	//          => CMissile::MoveFly()													//
	//----------------------------------------------------------------------------------//
	int MoveOnHardPoint(float dTime = 0);
	int MoveReleaseTime(float dTime = 0);
	int AstMoveReleaseTime(float dTime = 0);
	int MoveFly(float dTime = 0);

	//�̻��� ���� ����. 2004.11.11. 
	int CalHommingRotMtxAst(float dTime = 0.0f);
	int CalHommingRotMtx(float dTime = 0.0f);			//��ǥ�� ���� ȸ�� ��� ���.
	int MoveHomming(BOOL bHomming, float dTime = 0.0f);	//��ǥ�� �̻��� ���� �ϱ�.

public:

	// �⺻ �޼ҵ�.
	virtual int Move(float dTime=0);
	virtual int Draw(float dTime=0);   
	virtual void Release();

	// �߰� �޼ҵ�.
	const BOOL& IsReleased() const { return m_bIsReleased; }	//'������' �Ǿ���? 
	const BOOL& IsReleasedAst() const { return m_bIsReleasedAst; }	//'������' �Ǿ���? 
 	const BOOL& IsFired() const  { return m_bIsFired; }		//'���� ��ȭ' �Ǿ���? 
	
	//�̻��� �߻�-��Ŵ.
  	void SetFire();		

	const VECTOR3& GetSmokePos() const { return m_vSmokePosW; }
	float GetCurrVelo() const { return m_fCurrVelo; }		  
	//const float& GetCurrentSpeed() const { return m_fCurrVelo; }	
	const float& GetRocketFuel() const { return m_fRocketFuel; }		


	//�̻��� '����' �ִ� �������̵�. 2004.06.24.
	virtual BOOL DrawBoom(float dTime = 0);	

	//���� ����.: �������̵�. 2004.06.24+
	virtual void SetDead();			

	//�浹�Լ� �������̵�. 2004.06.24+.
	virtual BOOL CheckCollision(CUnit* pTarget, float fTargetRadius=0.0f);	

	//�̻��� ���� ����. 2004.11.11. 
	void LockOn(CUnit* pEmy);						//��ǥ�� Lock-On �Ѵ�. (��ǥ ����) 
	void LockOff(){ m_pTarget = NULL; }				//��ǥ ����.
	CUnit* GetTarget() const { return m_pTarget; }	



 };




/********************* end of file "Missile.h" *****************************/ 
