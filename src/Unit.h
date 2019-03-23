/////////////////////////////////////////////////////////////////////////////
//
// Unit.h:   �ֻ��� ���� Ŭ������ �ҽ� ����.
//
//
/////////////////////////////////////////////////////////////////////////////
 
#pragma once
 
#include "../../Engine/Ext/B3Yena_Ext.h"


class CCamera;	

typedef struct {
	//N/A
}UNITINFO, *LPUNITINFO;


#include "Frustum.h"	


//////////////////////////////////////////////////////////////////////////////
//
// ���� ���� �ִϸ��̼��� ���� ���� ����ü
//
typedef struct {

	VECTOR		vScale;			//���� ������.
	int			OldTime;		//���� �ִϸ� ���� �ð� ����.
	int			NowTime;

	int			Frm;			//���� �ִϽ� ����  ������ ī���� ����.
	int			TotFrm;			//���� �ִ� �� �����Ӽ�.
	int			FPS;			//���� �ִ��� '�ʴ� �����Ӽ�' : �ִϸ��̼� �ӵ�.

	SPRDATA*	pSpr;			//���� �ִ� �� ��������Ʈ ������.

	CCamera**	ppCamera;		//ī�޶�� ����������. : ������������ �ʿ�.	

}ANIINFO, *LPANIINFO;






//////////////////////////////////////////////////////////////////////////////
//
// �ֻ��� Ŭ���� : CUnit
// 
class CUnit  
{
protected:
	//
	// ������ ��ȯ ����
	//
	VECTOR3 m_vPos;			//������ ��ǥ
	VECTOR3	m_vRot;			//������ ȸ����.( �� �ະ ���Ȱ� )
	VECTOR3 m_vScale;		//������ ũ�� ��. ( �� �ະ)


	MATRIX m_mTranslation;
	MATRIX m_mRotation;
	MATRIX m_mScale;
	MATRIX m_mTM;			//������ ���� ��ȯ ���.


	//
	// ������ �Ϲ� ����.
	//
 	VECTOR3	m_vMoveVelo;		//�̵� ���ӵ�. : ������ �̵� �Ÿ� = �ð� * �̵����ӵ� 
	VECTOR3	m_vRotVelo;			//ȸ�� ���ӵ�. : ������ ȸ���� = �ð� * ȸ�����ӵ� 
	//VECTOR3 m_vWorldPosition;	//������ ���� ��ǥ. : ���ӻ��� '����'������ ��ǥ.
	
	// ������ ���� �̵�����.
	// ������� �ٸ��ǹ� : ex: �帮��Ʈ..
	VECTOR3 m_vDirection;		
	VECTOR3 m_vOrgDirection;		//�ʱ����.

	//
	// ������ �޽� ����.
	//
	CB3Mesh*	m_pMesh;

	//����̽� ������.
	LPDEVICE	m_pDev;


	//���� ������.
	static UINT m_UnitCnt;

	//
	//  '��ġ ����' �� ��� ����.
	//
	VECTOR3 m_vOrgPos;
	VECTOR3 m_vOrgRot;
	VECTOR3 m_vOrgScale;


	//
	// '�浹�� �ݰ�' ��� ������.
	//
	float	m_fCollRadius;		

	//
	// ���� �ִϸ� ���� ������.: 040623+
	//
	BOOL		m_bIsDead;			//�׾���...=> ���� �ִϸ����� �÷��� �̴�.
	ANIINFO		m_BoomAniInfo;		//���� �ִϸ��̼ǿ� ������...



	//
	// �ø��� ������. : 04.06.25.
	//
	//  '���','�̵�'�� ���õ� ���������� ���� ���̴�.
	//
	static CFrustum* m_pFrustum;			
	static BOOL		 m_bUseFrustumCulling;	//�ø� ����ϱ�. 


protected:

	//������ 
	//CUnit();
	CUnit(VECTOR3 vPos,				//'���� ��ġ'
		 VECTOR3 vRot,				//'ȸ��'
		 VECTOR3 vScale,			//������.
 		 VECTOR3 vMoveVelo,			//�̵��ӵ�.
		 VECTOR3 vRotVelo,			//ȸ�� �ӵ�.
		 
		 float		fRadius,		//�浹�� �ݰ�.
		 LPANIINFO	pBoomAniInfo,	//���� �ִϸ��̼� ���� ����ü.04.06.23+

		 CB3Mesh* pMesh,			//�޽� ������.
		 LPDEVICE pDev				//����̽� ������.
		 );


	// ���� �޼ҵ� :  TM �� �����.
	void SetTM(); 

public:

	//�Ҹ���.
	virtual ~CUnit();
	
	//���� ���� ����.
	static UINT& GetUnitCount();
	
	// �⺻ �޼ҵ�.
	virtual int Move(float dTime=0) = 0;
	virtual int Draw(float dTime=0) = 0;		 
	virtual void Release() = 0;

	// ��� ������.
	void SetPos(VECTOR3& rvPos) { m_vPos = rvPos; }			

	const VECTOR3& GetPos() const { return m_vPos; }
	const VECTOR3& GetRot() const { return m_vRot; }			
	const VECTOR3& GetMovVelo() const { return m_vMoveVelo;}	
	const VECTOR3& GetRotVelo() const { return m_vRotVelo;}
	

	const MATRIX& GetTransMtx() const { return m_mTranslation;}	
	const MATRIX& GetRotMtx() const   { return m_mRotation;}	
	const MATRIX& GetScaleMtx() const { return m_mScale;}		
 	const MATRIX& GetTM() const { return m_mTM; }

	//���� ���� ������.
	//void RotDir(VECTOR3* pvRot = NULL);
	//void RotDir(MATRIX* pmRot = NULL);
	const VECTOR3& GetDir() const { return m_vDirection; };	


	// ������ ������ǥ�� ���� �̵���Ű��.
	void WMove(VECTOR3 pos);
	void WRotate(VECTOR3 rot);
	void WScale(VECTOR3 scl);

	// ������ ��ġ�� ���½�Ų��.
	void Reset();




	//
	// �浹 üũ ����
	//
	enum {
		COLL_YES_ = YES_,		// ����!! �浹.
		COLL_NO_  =	NO_			// �浹 �ʉ���.
	};

	//�浹 �˻� �ϱ�.
	BOOL CheckCollision(CUnit* pTarget, float fTargetRadius=0.0f);
	//�浹 ���� �ݰ� ���ϱ�.
	const float& GetRadius() const { return m_fCollRadius; }			

	//
	// ���� �ִ� ����
	//
	//�׾��� - ���� ������ ����...?
	const BOOL& IsDead() const { return m_bIsDead; }				
	void SetDead();				//������ '�װ�' �մϴ�..=> ���� �ִ� ����.
 
	//���� �ִ� ��� : �⺻��Ÿ���� �ȴٸ�, �����ε� ���� : ������Ʈ.
	virtual BOOL	DrawBoom(float dTime=0);	 
	const ANIINFO&	GetAniInfo() const { return m_BoomAniInfo; }	


	//
	// �ø� ����.
	//
	//�ø��� ���� ȭ�鿡 ���̴� ������ �����Ǹ�  TRUE �� �����Ѵ�.
	const BOOL	IsVisible();						

	//�ø� ��ü �����ϱ�.
	static void InitFrustum(CFrustum* pFrustum)		
	{ m_pFrustum = pFrustum;  m_bUseFrustumCulling = YES_; }

	//�ܺο��� �ø� On/Off �ϱ�.
	static const BOOL& SetFrustumCullingState(BOOL bUse){ return (m_bUseFrustumCulling = bUse); }	

	//���� �ø� ���� ���.
	static const BOOL& GetFrustumCullingState(){ return m_bUseFrustumCulling; }			


};


/******************* end of file "Unit.h" *******************************/ 
