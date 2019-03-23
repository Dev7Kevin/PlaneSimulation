//////////////////////////////////////////////////////////////////////////
//
// PlasmaCannon.cpp : �ö�� �ɳ� �ҽ� ����.
//
//

//#include "../../Engine/B3x.h"
//#include "../../Engine_Ext/Engine.h"		//@fix. 2009.06.21.GS.only.
#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL ���� ���� �ʼ����. 

#include "Hero.h"
#include "Camera.h"
#include "plasmacannon.h"



///////////////////////////////////////////////////////////////////////////////
// �⺻ 1 ũ�⸦ ������ �� �޽� ����.
//
CPlasmaCannon::VTX_PLASMACANNON CPlasmaCannon::m_OrgVB[] = 
{
	{ VECTOR3(0.0f, 0.0f, 1.0f),  0.0f, 1.0f },	//v0.
	{ VECTOR3(0.0f, 0.5f, 0.5f),  0.0f, 0.7f },	//v1.
	{ VECTOR3(0.0f,-0.5f, 0.5f),  0.3f, 1.0f },	//v2.
	{ VECTOR3(0.0f, 0.5f,-0.5f),  0.7f, 0.0f },	//v3.
	{ VECTOR3(0.0f,-0.5f,-0.5f),  1.0f, 0.3f },	//v4.
	{ VECTOR3(0.0f, 0.0f,-1.0f),  1.0f, 0.0f }	//v5.

	 
};

int CPlasmaCannon::m_PCCount = 0;

#ifdef PLASMACANNON_TEST_
UINT CPlasmaCannon::m_PlasmaCannonTot = 0;
#endif


///////////////////////////////////////////////////////////////////////////////
//
CPlasmaCannon::CPlasmaCannon(
		LPDEVICE pDev,
		//CB3Mesh* pMesh,		//�Ѿ˿� �޽� ������. -> ������� ����
		SPRDATA* pSpr,			//�Ѿ˿� ��Ʈ����Ʈ ������.. 
								//pMesh, pSpr ���߿� �ϳ��� �־�� �Ѵ�.
		CCamera** ppCam,		//ī�޶� ���� ������. 2004.06.19. ����.

		float	fRadius,		// ���� ������ - '����'�� ������ �ȴ�.
		float	fLength,		// ���� ����. 
		UINT	Dmg,			// ������.
		COLOR	Color,			// �÷�.

		DWORD	ShowTime,		// '�����ֱ�' �ð�: �̽ð��� ���ϸ�, '�������.'
 		float	fAlpha,			// fade-out �� ���İ�.

		float	fFireSpeed,		//�߻� �ӵ�.
		VECTOR3 vFireDir,		//�߻� ����.
		VECTOR3 vUp,			//�߻��, '��ü�� 'up' ����.
		VECTOR3 vHorz,			//�߻��  '��ü�� ������'
 		float	fMaxRange,		//�ִ� ��Ÿ�...

		VECTOR3 vPos,			//�Ѿ� �߻� ��ġ.
		VECTOR3 vRot/* = VECTOR3(0.0f, 0.0f, 0.0f)*/, 
		VECTOR3 vScale /* = VECTOR3(1.0f, 1.0f, 1.0f)*/,
		VECTOR3 vMoveVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/,
		VECTOR3 vRotVelo/* = VECTOR3(0.0f, 0.0f, 0.0f)*/
		)
:CBullet(pDev, NULL, Dmg, fFireSpeed, vFireDir, fMaxRange, 
		pSpr, ppCam, Color,	
		fRadius, NULL,	 
		vPos, vRot, vScale, vMoveVelo, vRotVelo )
, m_fRadius(fRadius), m_fLength(fLength)
, m_ShowTime(ShowTime), m_OldShowTime(0), m_fAlpha(fAlpha)
{

	m_State = PC_SHOW_;
	m_OldShowTime = ::GetTickCount();

	// ���� �� ����.
	D3DXVec3Normalize(&m_vNView, &vFireDir);
	D3DXVec3Normalize(&m_vUp, &vUp); 
	//D3DXVec3Cross(&m_vHorz, &m_vUp, &m_vNView);
	D3DXVec3Normalize(&m_vHorz, &vHorz);

	m_vEndNView = m_vNView;
	//m_vEndUp = m_vUp;
	m_vEndHorz = m_vHorz;

	//���� ���� �����.
	m_pVB = new VTX_PLASMACANNON[6];
	ASSERT(m_pVB != NULL);


	++m_PCCount;


	//-------------------------------------------------------------------------- 
#ifdef PLASMACANNON_TEST_

 	m_PlasmaCannonSN = ++m_PlasmaCannonTot;

	B3D_Error(NO_, "[info] �ö�� ĳ�� ����[tot:%d SN:%d now:%d] : vPos(%.3f %.3f %.3f)	 vFireDir(%.3f %.3f %.3f)\n\
				   FireSpeed(%.2f %.2f %.2f),  fMax: %.3f vMaxRange(%.3f %.3f %.3f)",
				   m_PlasmaCannonTot, m_PlasmaCannonSN, m_PCCount, 
				   m_vPos.x, m_vPos.y, m_vPos.z, m_vFireDir.x, m_vFireDir.y, m_vFireDir.z,
				   m_vMoveVelo.x,  m_vMoveVelo.y,  m_vMoveVelo.z,  
				   m_fMaxRange, m_vMaxRange.x, m_vMaxRange.y, m_vMaxRange.z);

#endif
	//-------------------------------------------------------------------------- 

}



///////////////////////////////////////////////////////////////////////////////
//
CPlasmaCannon::~CPlasmaCannon(void)
{
	//-------------------------------------------------------------------------- 
#ifdef PLASMACANNON_TEST_

	B3D_Error(NO_, "[info] �ö�� ĳ�� ���ŵ�[tot:%d SN:%d now:%d] : vPos(%.3f %.3f %.3f) vFireDir(%.3f %.3f %.3f)\n\
				   FireSpeed(%.2f %.2f %.2f),  fMax: %.3f vMaxRange(%.3f %.3f %.3f)",
				   m_PlasmaCannonTot, m_PlasmaCannonSN, m_PCCount, 
				   m_vPos.x, m_vPos.y, m_vPos.z, m_vFireDir.x, m_vFireDir.y, m_vFireDir.z,
				   m_vMoveVelo.x,  m_vMoveVelo.y,  m_vMoveVelo.z,  
				   m_fMaxRange, m_vMaxRange.x, m_vMaxRange.y, m_vMaxRange.z);

#endif
	//-------------------------------------------------------------------------- 


	Release();

}





///////////////////////////////////////////////////////////////////////////////
//
int CPlasmaCannon::Move(float dTime/* =0 */)
{
	if(!CBullet::Move(dTime))
		return FALSE;


	//�ø��� ������ ������..�׸��� �ʴ´�. 2004.06.25.
	if(!IsVisible()) return TRUE;	



	switch(m_State)
	{
	case PC_SHOW_: 
		return Show(dTime);

	case PC_DISAPPEAR_:
		return Disappear(dTime);

	case PC_DELETE_:
		return FALSE;		//�޸� ���Ÿ� ���� FALSE ����.
 
	}

	return TRUE; 
}



///////////////////////////////////////////////////////////////////////////////
//
int CPlasmaCannon::Show(float dTime)
{
	//1. ī�޶���� '�ü�' ���� ���ϱ�.
	VECTOR3 vCamNView = (*m_ppCamera)->GetEye() - m_vPos; 
						//m_vPos - (*m_ppCamera)->GetEye();
	D3DXVec3Normalize(&vCamNView, &vCamNView);

	//--------------------------
	//#0. �� ���� ������.
	//--------------------------

	// ī�޶���� ������ ��� ����.
	
	//0. �������� �� ��� ( ������� = ī�޶� �ü����� x ���� ���� (�߻� ����))
 	D3DXVec3Cross(&m_vUp, &m_vNView, &vCamNView);
	D3DXVec3Normalize(&m_vUp, &m_vUp);

	//1. ������ �� ���. ( ������� = ���溤�� x ��������)
	D3DXVec3Cross(&m_vHorz, &m_vUp, &m_vNView);
	D3DXVec3Normalize(&m_vHorz, &m_vHorz);

	//2. ������ ��� �����.
	MATRIX mRotB;
	D3DXMatrixIdentity(&mRotB);

	mRotB._11 = m_vHorz.x;	mRotB._12 = m_vHorz.y;	mRotB._13 = m_vHorz.z;
	mRotB._21 = m_vUp.x;	mRotB._22 = m_vUp.y;	mRotB._23 = m_vUp.z;
	mRotB._31 = m_vNView.x;	mRotB._32 = m_vNView.y;	mRotB._33 = m_vNView.z;


	//--------------------------
	//#1. �� '����' ������.
	//--------------------------
	// ...........



	//--------------------------
	//0. TM �����. 
	// : ���� �β� - Y�� ������, ���� ���� - z �� ������. : x�� �������� ����.
	//--------------------------
	D3DXMatrixScaling(&m_mScale, 1.0f, m_fRadius, m_fLength);
	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);
	m_mTM = m_mScale * mRotB * m_mTranslation;


	//1. �� ��ü ������...(v1~v4)
 	for(int i=0; i<6; ++i)
	{
		D3DXVec3TransformCoord(&m_pVB[i].vPos, &m_OrgVB[i].vPos, &m_mTM);
		m_pVB[i].U = m_OrgVB[i].U;
		m_pVB[i].V = m_OrgVB[i].V;
	}

	//2. ���� '����'�� ������...
 	//..... 
  

	return TRUE;
}





///////////////////////////////////////////////////////////////////////////////
//
int CPlasmaCannon::Disappear(float dTime)
{
	//5. ���İ��� 0.01 ���ϸ�, '����'�� ���� false ����.

	return TRUE;
}

 





///////////////////////////////////////////////////////////////////////////////
//
int CPlasmaCannon::Draw(float dTime/*=0*/)   
{

	//�ø��� ������ ������..�׸��� �ʴ´�. 2004.06.25.
	if(!IsVisible()) return TRUE;	


	//------------------------------------------------------------------
	//���� �׽�Ʈ OFF.
	g_B3pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
 
 	// ���� ���� ON.
 	g_B3pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_B3pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	g_B3pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);	

	//------------------------------------------------------------------
	// LIGHTS ON!
	g_B3pDevice->SetRenderState(D3DRS_LIGHTING, TRUE); 
	g_B3pDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff); 


	//------------------------------------------------------------------
	// ��Ÿ �ɼ�.
	g_B3pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	 
	g_B3pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);  


	//-----------------------------------------------------------------
	// ��Ƽ���� ����.
	D3DMATERIAL9  mat;
	ZeroMemory(&mat, sizeof(mat));
	mat.Ambient.r = mat.Diffuse.r = m_Color.r; 
	mat.Ambient.g = mat.Diffuse.g = m_Color.g;
	mat.Ambient.b = mat.Diffuse.b = m_Color.b;
	mat.Ambient.a = mat.Diffuse.a = m_Color.a;
	g_B3pDevice->SetMaterial(&mat);

 
	//-----------------------------------------------------------------
	g_B3pDevice->SetFVF(FVF_PLASMACANNON_);
	g_B3pDevice->SetTexture(0, g_bWireMode?NULL:m_Spr.pTexture);
 	 
	
	//-----------------------------------------------------------------
	//���� ��� ����.: �̹� ��ȯ����.
	MATRIX m; D3DXMatrixIdentity(&m);
	g_B3pDevice->SetTransform(D3DTS_WORLD, &m);

	//-----------------------------------------------------------------
	//������.
 	g_B3pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 4, m_pVB, sizeof(VTX_PLASMACANNON));
	 


	//-------------------------------------------------------------------
	// ������ �ɼ� ����.
	g_B3pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_B3pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	g_B3pDevice->SetRenderState(D3DRS_LIGHTING, FALSE); 
	g_B3pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	 
	g_B3pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);		 
	 
 

	return TRUE;
}



///////////////////////////////////////////////////////////////////////////////
//
void CPlasmaCannon::Release()
{	 
	SAFE_DELARRY(m_pVB); 

	--m_PCCount;
}







//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// namespace GP
//
//////////////////////////////////////////////////////////////////////////

 #include "list"
namespace GP
{
	SPRDATA	_sprPlasmaCannon;
	
	
	list<CPlasmaCannon*> _PlasmaCannonList;
	typedef list<CPlasmaCannon*>::iterator PC_Itor;

	VECTOR3 _vPCFirePos[4] = { 
		VECTOR3( -3.5f,  1.2f, -4.0f),
		VECTOR3( -3.5f, -1.2f, -4.0f),
		VECTOR3(  3.5f,  1.2f, -4.0f),
		VECTOR3(  3.5f, -1.2f, -4.0f),
 	};

	UINT _PCFirePosCnt = 0;
	const UINT _PCMax = 100;		//�ִ� 100��.
};





///////////////////////////////////////////////////////////////////////////////
//
BOOL GP::PlasmaCannonInit()
{
	if(!B3DSprite_Load(&_sprPlasmaCannon, "..\\data\\PlasmaCannon_2.jpg"))
	{
		return FALSE;
	}


	_PlasmaCannonList.clear();
	 
	return TRUE;
}






///////////////////////////////////////////////////////////////////////////////
//
void GP::PlasmaCannonCreate(CHero* pHero, CCamera** ppCamera)
{
	if(CPlasmaCannon::GetPCCount() > _PCMax ) return;

	if(++_PCFirePosCnt >=4) _PCFirePosCnt = 0;

	//�߻� ��ġ ���.
	VECTOR3 vPos =  pHero->GetHorz()*_vPCFirePos[_PCFirePosCnt].x +
					pHero->GetUp()*_vPCFirePos[_PCFirePosCnt].y +
					pHero->GetNView()*_vPCFirePos[_PCFirePosCnt].z ;

	vPos += pHero->GetPos();
					
		  
	
	CPlasmaCannon* pPC = new CPlasmaCannon(
		g_B3pDevice, 
		&_sprPlasmaCannon, 
		ppCamera, 
		2.0f,						//�β�.
		5.0f,						//����.
		10,							//������.
		COLOR(1.0f, 0.3f, 0, 1.0f),	//����.
		1000,			//show time.
		1.0f,			//alpha.
		fabs(pHero->GetCurrDirVelo().z) + 500.0f,	//fire speed. 
		pHero->GetNView(),
		pHero->GetUp(),
		pHero->GetHorz(),
		1000.0f,		//max range.
		vPos 
		);
	ASSERT(pPC != NULL);

				
	_PlasmaCannonList.push_back(pPC);
 

}





///////////////////////////////////////////////////////////////////////////////
//
void GP::PlasmaCannonMove(float dTime)
{
	for(PC_Itor i = _PlasmaCannonList.begin();  i != _PlasmaCannonList.end(); )
	{
		if((*i)->Move(dTime) == FALSE)
		{
			delete (*i);
			_PlasmaCannonList.erase(i++);
		}
		else ++i;
	}
}





///////////////////////////////////////////////////////////////////////////////
//
void GP::PlasmaCannonDraw(float dTime)   
{
	for(PC_Itor i = _PlasmaCannonList.begin();  i != _PlasmaCannonList.end(); ++i)
	{
		(*i)->Draw(dTime); 
	}
}





///////////////////////////////////////////////////////////////////////////////
//
void GP::PlasmaCannonRelease()
{
	for(PC_Itor i = _PlasmaCannonList.begin(); i != _PlasmaCannonList.end();)
	{
		delete (*i);				//�޸� ����.
		_PlasmaCannonList.erase(i++);		//������ ����.
	}

	//���� �� ����.
 	B3DSprite_Release(&_sprPlasmaCannon);
}



 


/*************** end of file "PlasmaCannon.cpp " ***************************/



















