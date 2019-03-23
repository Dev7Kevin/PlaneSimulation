/////////////////////////////////////////////////////////////////////////////
//
// Unit.cpp:   �ֻ��� ���� Ŭ������ �ҽ� ����.
//
//
/////////////////////////////////////////////////////////////////////////////

#include "Unit.h"
#include "Camera.h"	// ���� �ִ� �������� ����.. 


//////////////////////////////////////////////////////////////////////
//
UINT CUnit::m_UnitCnt = 0;



// �ø���. 2004.06.25.
CFrustum*	CUnit::m_pFrustum = NULL;
BOOL		CUnit::m_bUseFrustumCulling = YES_;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnit::CUnit(VECTOR3 vPos, 
			 VECTOR3 vRot, 
			 VECTOR3 vScale,
 			 VECTOR3 vMoveVelo,
			 VECTOR3 vRotVelo,

			 float	 fRadius,			//�浹�� �ݰ�.
			 LPANIINFO	pBoomAniInfo,	//���� �ִϸ��̼� ���� ����ü.04.06.23+

			 CB3Mesh* pMesh,
			 LPDEVICE pDev
			 ):
m_vPos(vPos), m_vRot(vRot), m_vScale(vScale),
m_vMoveVelo(vMoveVelo), m_vRotVelo(vRotVelo),
m_fCollRadius(fRadius),  
m_pMesh(pMesh), m_pDev(pDev)
{
	//
	// �浹 ����..
	//
	m_bIsDead = NO_;

	//
	// ���� �ִ� ���� ���� : �ܺο��� �־��� �����Ͱ� ���ٸ�, �׳� 'Ŭ����'
	//
	ZeroMemory(&m_BoomAniInfo, sizeof(ANIINFO));
	if(pBoomAniInfo != NULL) m_BoomAniInfo = *pBoomAniInfo;


	SetTM();		


	//���� ���� ī��Ʈ ����.
	++m_UnitCnt;
	

	//������ ����.
	m_vOrgPos = m_vPos;
	m_vOrgRot = m_vRot;
	m_vOrgScale = m_vScale;	

	// ���� ����. �Ϲ������ -1 �� �������� ó���Ѵ�.
	m_vOrgDirection = m_vDirection = VECTOR3(0, 0, -1);		
 

	// �ø� ������.
	m_bUseFrustumCulling = YES_;		//�⺻...�翬!! 
	//m_bIsVisible = YES_;				//�ϴ� ������ ȭ�鿡 ���̴� ������!!
}


////////////////////////////////////////////////////////////////////////////
//
CUnit::~CUnit()
{

	Release();

	//���� ���� ī��Ʈ ����.
	--m_UnitCnt;

}




/////////////////////////////////////////////////////////////////////////////
//
//  const BOOL CUnit::IsVisible() 
//
//  �ø� �˻� - �˻��� �þ߿� ��ü�� ���� ������ TRUE �� �����ϰ�..
//  �ݴ��� ���� FALSE  �� �����Ѵ�. 
//
const BOOL CUnit::IsVisible()
{
	if(m_bUseFrustumCulling == FALSE) return TRUE;		// �ø��� ������� ������ true ����!! => ��� �׸���~!!

	static MATRIX mView;		
	m_pDev->GetTransform(D3DTS_VIEW, &mView);		

	return  m_pFrustum->IsVisible(&mView, &m_vPos, m_fCollRadius);
	//m_bIsVisible = m_pFrustum->IsVisible(&mView, &m_vPos, m_fRadius);
	//return m_bIsVisible;
}





////////////////////////////////////////////////////////////////////////////
//
// void CUnit::SetDead() 
//
// ������ '�װ���' -> ���� �ִϸ��̼���  �����ϰ� �Ѵ�.
//
void CUnit::SetDead()
{
	m_bIsDead = TRUE;

	//���� �ִ� ������ ����
	m_BoomAniInfo.NowTime = ::timeGetTime();		
	m_BoomAniInfo.OldTime = ::timeGetTime();
	m_BoomAniInfo.Frm = 0;

}




  


////////////////////////////////////////////////////////////////////////////
//
// BOOL CUnit::DrawBoom() - ���� �ִϸ��̼� ������. 
//
// �ִϰ� ����Ǹ� �޸𸮸� �����ϱ� ���� FALSE �� ���ϵȴ�.
//
// �⺻ ���Ĵ� ������ ���� ��ġ�� �߽����� ������ ��Ÿ�Ϸ� ��µȴ�.
// ���� ����Ŭ�������� �����ε��� ����� �����ϴ�.
//
BOOL CUnit::DrawBoom(float dTime)	
{
	m_BoomAniInfo.NowTime = timeGetTime();

	if(m_BoomAniInfo.NowTime  - m_BoomAniInfo.OldTime > m_BoomAniInfo.FPS)
	{
		m_BoomAniInfo.OldTime = m_BoomAniInfo.NowTime; 
		if(++m_BoomAniInfo.Frm >=  m_BoomAniInfo.TotFrm ) 
		{
			m_BoomAniInfo.Frm = 0;

			//�ִϸ��̼��� ������ ������ �����϶�� �ǹ̷� FALSE �� �����Ѵ�.
			return FALSE;				
		}
	}



	//�ø��� �õ�...'�þ�'���� �����..�������� ���� �ʴ´�.
	//�������� ȿ���� ���� '����'�� �״�� ���ܵ�...2004.06.25+  
	//if(!IsVisible())	return TRUE;		<-- �ּ�ó�� �մϴ�.


	// ȸ������ ī�޶��� 'ȸ�����'�� ��´� : �⺻���� '�Ϲ�' ������..
	m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm].mRot = (*m_BoomAniInfo.ppCamera)->GetBillMtx();

	B3DSprite_Scaling(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], m_BoomAniInfo.vScale.x, m_BoomAniInfo.vScale.y);
	B3DSprite_Translation(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], m_vPos.x, m_vPos.y, m_vPos.z, ON3D_);
	B3DSprite_DrawFX(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], COLOR(1.0f, 1.0f, 1.0f, 1.0f));

	//'����' ���� �߽� �κ��� ���� ��� ó���� ���ؼ�... 2004.06.24 
	B3DSprite_Draw3D(&m_BoomAniInfo.pSpr[m_BoomAniInfo.Frm], TRUE, 0.2f, NOZWRITE_);	 

	return TRUE;

}//end of BOOL CUnit::DrawBoom()








////////////////////////////////////////////////////////////////////////////
//
// BOOL CUnit::CheckCollision() : �浹 �˻�.
//
// �� ������ '����' ��ġ�� �Ÿ��� 
// ��ǥ ������ �浹���� �ݰ溸�� ������ �浹�̴�...
//
BOOL CUnit::CheckCollision(CUnit* pTarget,				//��ǥ ���� ������.
						   float fTargetRadius/*=0.0f*/	//��ǥ ������ �浹 �ݰ�.
						   )
{
	// �� ���� ������ �Ÿ��� ���Ѵ�.
	float fDistance = D3DXVec3Length(&(m_vPos - pTarget->GetPos()));

	//�־��� �ݰ��� ���ٸ� ��ǥ������ �ݰ��� ����Ѵ�.
	if(fTargetRadius == 0.0f) fTargetRadius = pTarget->GetRadius();		


	// �� ������ �浹���� �Ÿ��� ���� ���Ѵ�.
	float fTotRadius = m_fCollRadius + fTargetRadius;


	// ��ǥ ������ �浹���� �ݰ溸�� ������ �浹....
	if( fDistance < fTotRadius)
	{
		return  COLL_YES_;			// �浹�� �ǹ�.
	}

	return COLL_NO_;				// �浹 �ƴ�.
}






////////////////////////////////////////////////////////////////////////////
//
UINT& CUnit::GetUnitCount()
{
	return m_UnitCnt;
}


////////////////////////////////////////////////////////////////////////////
//
void CUnit::Release()
{
	//N/A
}




////////////////////////////////////////////////////////////////////////////
//
// ��ȯ ��� �����.
//
void CUnit::SetTM()
{
	D3DXMatrixRotationYawPitchRoll(&m_mRotation, m_vRot.y, m_vRot.x, m_vRot.z);
	D3DXMatrixScaling(&m_mScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);

	MATRIX temp;
	D3DXMatrixMultiply(&temp, &m_mScale, &m_mRotation);
	D3DXMatrixMultiply(&m_mTM, &temp, &m_mTranslation);
 	
}





////////////////////////////////////////////////////////////////////////////
//
void CUnit::WMove(VECTOR3 pos)
{
	m_vPos = pos;

	SetTM();
}

////////////////////////////////////////////////////////////////////////////
//
void CUnit::WRotate(VECTOR3 rot)
{
	m_vRot = rot;

	SetTM();
}


////////////////////////////////////////////////////////////////////////////
//
void CUnit::WScale(VECTOR3 scl)
{
	m_vScale = scl;

	SetTM();
}


////////////////////////////////////////////////////////////////////////////

void CUnit::Reset()
{
	m_vPos = m_vOrgPos;
	m_vRot = m_vOrgRot;
	m_vScale = m_vOrgScale;

	m_vMoveVelo = VECTOR3(0, 0, 0);		//�ӵ� �ʱ�ȭ.
	m_vRotVelo = VECTOR3(0, 0, 0);

	m_vDirection = m_vOrgDirection;			


	SetTM();

}







/****************** end of file "Unit.cpp" ********************************/