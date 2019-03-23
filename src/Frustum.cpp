// Frustum.cpp: implementation of the CFrustum class.
// 
//
//
////////////////////////////////////////////////////////////////////////////// 


#include "Frustum.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
// �־��� 3���� ��(vP0, vP1, vP2) �� ���� '���'�� ����. 
//
void PLANE::PlaneFromPoint(VECTOR3& vP0, VECTOR3& vP1, VECTOR3& vP2)
{
	VECTOR3 V0, V1, N;
	V0 = vP1 - vP0;
	V1 = vP2 - vP0;
	D3DXVec3Cross(&N, &V0, &V1);
	D3DXVec3Normalize(&N, &N);

	_A = N.x;
	_B = N.y;
	_C = N.z;
	_D = 0;  
}




//////////////////////////////////////////////////////////////////////////////
//
// class CFrustum  Interfaces. 
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
CFrustum::CFrustum()
{
	m_fDist = 0.0f;
	//N/A
}
 
//////////////////////////////////////////////////////////////////////////////
//
CFrustum::~CFrustum()
{ 
	//...
}

//////////////////////////////////////////////////////////////////////////////
//
CFrustum::CFrustum(float fovY, float ratio, float n, float f)
{
	m_fDist = 0.0f;

	Init( fovY, ratio, n, f);
}




//////////////////////////////////////////////////////////////////////////////
//
// �þ� ����ü �ø��� ���� 6��ü�� �����մϴ�.
// ��, ���� ���ÿ����� �����, ������� �����մϴ�.
//
void CFrustum::Init(float fovY, float ratio, float n, float f)
{	
	
	// ���� �þ߰� ���.
	float fovX = fovY * ratio;


	// �������� ���� ���� 5��.
	// ���������� 'p0' ���� �ٶ󺼰���� ����������
	/*
    p1��������������p2 
	  ��          ��  
	  ��    p0    �� 
	  ��          �� 
	p3��������������p4
	*/	
	VECTOR3 P[5];
	P[0] = VECTOR3(0, 0, 0);		//���������� ����.
   

	
 

	//--------------------------//
	// ������� �ø��� ����.	//
	//--------------------------//
	m_fNear = n;		

	//--------------------------//
	// ������� �ø��� ����.	//
	//--------------------------//
	m_fFar = f;
	

	//--------------------------//
	// ���� ��� ����.			//
	//--------------------------//
 	P[1] = VECTOR3(-f * ((float)tan(fovX * 0.5f)), 
		  		    f * (float)tan(fovY * 0.5f), 
				    f );
	P[3] = VECTOR3(P[1].x, -P[1].y, P[1].z);
	m_Left.PlaneFromPoint(P[0], P[3], P[1]);	//��������

	//--------------------------//
	// ������ ��� ����.		//	
	//--------------------------// 
	P[2] = VECTOR3(f * ((float)tan(fovX * 0.5f)), 
		  		   f * (float)tan(fovY * 0.5f), 
				   f);
	P[4] = VECTOR3(P[2].x, -P[2].y, P[2].z);
	m_Right.PlaneFromPoint(P[0], P[2], P[4]);	//��������


	//--------------------------//
	// ��� ��� ����.			//
	//--------------------------// 
	m_Top.PlaneFromPoint(P[0], P[1], P[2]);		//��������

	//--------------------------//
	// �Ϻ� ��� ����.			//
	//--------------------------// 
	m_Bottom.PlaneFromPoint(P[0], P[4], P[3]);	//��������



	//�������� ����ȭ �غ�.
	_InitMesh( P );

}








//////////////////////////////////////////////////////////////////////////////
//
// int CFrustum::IsVisible(...) : �� ���� ���� ���ü�����
//
// ��ü ( �� p2 �� �ݰ� radius �� ������..) �� View ��ȯ ��ķ� ������
// Frustum �ȿ� �ֽ��ϴ�.�� 1 �� �����մϴ�.
//
int CFrustum::IsVisible(MATRIX* pViewTM, VECTOR3* pTargetPos, float radius) 
{
	//
	// ��ǥ�� ��ġ�� ��������� ����.
	//
	// * (ī�޶� ��ȯ ���) x (���� ����) = (ī�޶� ������ ����)
	// * ��, Frustum Culling �� �ֿ� ����� ī�޶� ������ ������ 
	// * Frumtum �ȿ� �ֳ�? ����? �� �����ϴ� �� �ֽ��ϴ�..
	//
	VECTOR3 vPos;
	D3DXVec3TransformCoord(&vPos, pTargetPos, pViewTM);

	m_fDist = 0.0;

	//--------------------------//
	// '��/�� ���'�� �ø�...	//	
	//--------------------------//
  	if( vPos.z > (m_fFar + radius)) return FALSE;			//'��' �����̹Ƿ�, '����Ÿ�'�� ���մϴ�.
	if( vPos.z < (m_fNear - radius)) return FALSE;
	
  
	//--------------------------//
	//'��/ ���� ���'�� �ø�... // 
	//--------------------------// 
	//VECTOR4 pos = VECTOR4(vPos, 1);						//4���� ������ ���ϰ���� �������⿡, 3���г����� ����ϰڽ��ϴ�.

	m_fDist = D3DXVec3Dot(&vPos, (VECTOR3*)&m_Left);		//������ �Ÿ������ '����'�� �����մϴ�.
  	//m_fDist = vPos.x * m_Left._A + vPos.z * m_Left._C;	//��������� ��� : Y �� ������ �񱳴� �ǹ̰� �����Ƿ� ������ �� �ֽ��ϴ�.
	if(m_fDist > radius) return FALSE;						//������ �Ÿ��� '�ݰ�' ���� ũ�ٸ� �ۿ� �ִ� ���Դϴ�.

	m_fDist = D3DXVec3Dot(&vPos, (VECTOR3*)&m_Right);
 	//m_fDist = vPos.x * m_Right._A + vPos.z * m_Right._C;	
 	if(m_fDist > radius) return FALSE;


	//--------------------------//
	//'��/�� ���'�� �ø�...    //
	//--------------------------//
	m_fDist = D3DXVec3Dot(&vPos, (VECTOR3*)&m_Top);
 	//m_fDist = vPos.y * m_Top._B + vPos.z * m_Top._C;		//��������� ��� : Y �� ������ �񱳴� �ǹ̰� �����ϴ�.
 	if( m_fDist > radius) return FALSE;

	m_fDist = D3DXVec3Dot(&vPos, (VECTOR3*)&m_Bottom);
	//m_fDist = vPos.y * m_Bottom._B + vPos.z * m_Bottom._C;	
 	if( m_fDist > radius) return FALSE;

 
	return TRUE;

}










///////////////////////////////////////////////////////////////////////////////
//
//  �������� ����ȭ ���۱���.
//
//
void CFrustum::_InitMesh(VECTOR3 p[5])
{
	
	memcpy(m_vFPos, p, sizeof(VECTOR3) * 5);
	 

	//�ε��� ����.	 
	USHORT index [] = {
		 0, 3, 1 ,
		 0, 1, 2 ,
		 0, 2, 4 ,
		 0, 3, 4 ,
		 1, 2, 3 ,
		 3, 2, 4 ,
	};

	memcpy(m_pFIndex, index, sizeof(index)); 
	 
}







///////////////////////////////////////////////////////////////////////////////
//
//  �������� ����ȭ : 
//
//
int CFrustum::Draw(LPDEVICE pDev, COLOR col, MATRIX* pMtx/*= NULL*/, BOOL bWire/*=TRUE*/)
{


	DWORD oldmode; 
	pDev->GetRenderState(D3DRS_FILLMODE, &oldmode);

	MATERIAL m;
	ZeroMemory(&m, sizeof(m));
	m.Diffuse = m.Ambient = col;
	pDev->SetMaterial(&m);
	pDev->SetTexture(0, NULL);
	DWORD oldlight;
	pDev->GetRenderState(D3DRS_LIGHTING, &oldlight); 
	pDev->SetRenderState(D3DRS_LIGHTING, TRUE); 
	pDev->SetRenderState(D3DRS_AMBIENT, 0xffffffff);
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);	 
	if(pMtx) pDev->SetTransform(D3DTS_WORLD, pMtx);
	pDev->SetFVF( D3DFVF_XYZ );

	if(bWire == FALSE)
	{		
		pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID); 		
		pDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 6, m_pFIndex, 
									  D3DFMT_INDEX16, m_vFPos, sizeof(VECTOR3));
	}
	 	
	pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 6, m_pFIndex, 
									D3DFMT_INDEX16, m_vFPos, sizeof(VECTOR3));
	
	 
	pDev->SetRenderState(D3DRS_FILLMODE, oldmode);
	pDev->SetRenderState(D3DRS_LIGHTING, oldlight); 
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	 
 	pDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);


	return TRUE;
}
 

 



/****************** end of file "Frustum.cpp" *********************************/

