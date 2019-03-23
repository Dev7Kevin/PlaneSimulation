//
// Instrument.cpp :  ���� �ҽ� ����.
//
//
//////////////////////////////////////////////////////////////////////////////


#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL ���� ���� �ʼ����. 


#include "Unit.h"
#include "Plane.h"
#include "Hero.h"
#include "Enemy.h"				

#include "Bullet.h"
#include "Shockwave.h"
#include "Missile.h"


#include "Instrument.h"
#include "list"					
using namespace std;


//////////////////////////////////////////////////////////////////////////////
//
namespace GP
{
	//
	// �ڼ��� - ADI (Attitude Director Indicator) ����. 2004.07.02. 
	//
	CB3Mesh*	_pMeshADI = NULL;
	MATRIX		_mTMADI;
	SPRDATA		_sprADIBody;
 

	//
	// ���� ǥ�ñ� - WI (Weapon Indicator) ����. 2004.07.02.  
	//
	SPRDATA	_sprWIBody;
	SPRDATA _sprMissileIcon;

	
	//
	// 2D ���̴�.
	//
	SPRDATA _sprRWRBody;
	SPRDATA _sprRWRDot;

	typedef list<CEnemy*>::iterator EmyItor_;
	extern list<CEnemy*> _EnemyList;				//���� ����Ʈ.
	extern	CHero*	_pHero;

	typedef list<VECTOR3>::iterator RWREmyItor_;
	list<VECTOR3> _RWREmyList;						//��ĵ�� ���� ����Ʈ.(��ġ�� �ʿ�)



};








///////////////////////////////////////////////////////////////////////////
// 
// 2D Radar ����.	
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// 
void GP::RWRLoad() 	
{

	B3DSprite_Load(&_sprRWRBody, "RWR_body.bmp");
	B3DSprite_Scaling(&_sprRWRBody, 
		_sprRWRBody.Width, 
		_sprRWRBody.Height);		//���� ������ ��ŭ �����ϸ�.


	B3DSprite_Load(&_sprRWRDot, "RWR_Dot.bmp");
	B3DSprite_Scaling(&_sprRWRDot, 
		_sprRWRDot.Width, 
		_sprRWRDot.Height);		//���� ������ ��ŭ �����ϸ�.


}

///////////////////////////////////////////////////////////////////////////
// 
void GP::RWRUpdate(float dTime /* = 0.0f */)	
{
	//0.��ĵ ����Ʈ Ŭ����.
	_RWREmyList.clear();


	//1.���ΰ��� ���� ��ġ�� �������� ���� �ݰ�(5km) ���⸦ �˻�
	//  ����Ʈ�� ����Ѵ�...
	const float RWR_RADIUS = 5000.0f;

	for(EmyItor_ i= _EnemyList.begin(); i != _EnemyList.end(); i++)
	{
		CEnemy* pEmy = *i;
		if(pEmy->CUnit::CheckCollision(_pHero, RWR_RADIUS) == CUnit::COLL_YES_)
		{
			//�浹..��, ����� �����ȿ� ��� �����Ƿ�, �� ������ ��ġ��
			//����Ʈ�� ����Ѵ�...
			_RWREmyList.push_back(pEmy->GetPos());
		}
	}


	//2.��ĵ�� �ѵ��� ��ġ�� ���ΰ� �߽����� �� ��ġ�Ѵ�.
	//
	MATRIX mHeroView;

	//���ΰ� ���� ��ġ�� �߽� ���ؽ�.
	/*VECTOR3 vLookAt = _pHero->GetModelNView()*10.0f;
	vLookAt.y = 0.0f;
	vLookAt += _pHero->GetPos();

	D3DXMatrixLookAtLH(&mHeroView, 
	&_pHero->GetPos(), 
	&vLookAt, 
	&VECTOR3(0, 1, 0)	
	);*/


	//���ΰ� �� ������ �߽��� ���� ��ȯ��� ����..
	D3DXMatrixLookAtLH(&mHeroView, 
		&_pHero->GetPos(), 
		&(_pHero->GetPos() + _pHero->GetNView()*10.0f), 
		&_pHero->GetUp()  //&VECTOR3(0, 1, 0)	
		);

	float ratio = (_sprRWRBody.Width * 0.5f) / RWR_RADIUS;

	for(RWREmyItor_ i= _RWREmyList.begin();  i!= _RWREmyList.end(); i++)
	{		 
		VECTOR3 vPos = (*i);

		//������ ���ΰ� �߽����� ����, �� ���ΰ� �������� �����Ѵ�.
		D3DXVec3TransformCoord(&vPos, &vPos, &mHeroView);

		//RWR �̹����� '����' �ȿ� ������, ��ǥ�� �����Ѵ�.
		vPos *= ratio;
		(*i) = vPos;			//������ ��ǥ ����.				 
	}


}






///////////////////////////////////////////////////////////////////////////
// 
void GP::RWRDraw()	
{
	//���� ǥ�ñ� ��ü ���.
	VECTOR3 vDrawPos = VECTOR3( _sprWIBody.Width + 3, 
		g_B3Mode.Height - (_sprRWRBody.Height+2), 1.0f);
	B3DSprite_Translation(&_sprRWRBody, vDrawPos.x, vDrawPos.y, vDrawPos.z);
	B3DSprite_Draw(&_sprRWRBody, NO_, YES_, 0.5f);



	//���̴��� ��ĵ�� �� ��ü ���... 
	//����Ʈ ó���� ���� �� �����ϰ� �� �� �ְ�����, ���⼭�� �� ����Ѵ�.
	VECTOR3 vOrgPos = vDrawPos + VECTOR3(_sprRWRBody.Width * 0.5f, _sprRWRBody.Height * 0.5f, 0.0f);

	for(RWREmyItor_ i= _RWREmyList.begin();  i!= _RWREmyList.end(); i++)
	{
		//CEnemy* pEmy = *i;		
		VECTOR3 vPos;
		vPos = vOrgPos + VECTOR3( (*i).x, -(*i).z, 0.0f); //(*i).y);

		B3DSprite_Translation(&_sprRWRDot, vPos.x, vPos.y, 1.0f); //vPos.z);
		//B3DSprite_DrawFX(&_sprRWRDot, COLOR(0, 0, 1, 1)); 
		B3DSprite_Draw(&_sprRWRDot);

	}

}






///////////////////////////////////////////////////////////////////////////
// 
void GP::RWRRelease()		
{
	B3DSprite_Release(&_sprRWRBody);
	B3DSprite_Release(&_sprRWRDot);

	_RWREmyList.clear();

}









///////////////////////////////////////////////////////////////////////////
// 
// ���� ǥ�ñ� (Weapon Indicator)  ����.
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// 
// �̻��� ������. : '�����'�� ���� ǥ�ñ�� ����� ���̴�. 
// 2D ���� �������� ����Ұ���. 
// 2004.07.02._pMeshADI 
//
void GP::WeaponIndicatorLoad()
{
	B3DSprite_Load(&_sprMissileIcon, "�̻���_������.bmp");

	B3DSprite_Load(&_sprWIBody, "�̻���_ǥ�ñ�_��ü.bmp");
 	B3DSprite_Scaling(&_sprWIBody, 
					_sprWIBody.Width, 
					_sprWIBody.Height);		//���� ������ ��ŭ �����ϸ�.


}

///////////////////////////////////////////////////////////////////////////
// 
//  ���� ǥ�ñ� ���._pMeshADI
//
void GP::WeaponIndicatorDraw(CMissile* pMissile[], DWORD MissileMax)
{
	//���� ǥ�ñ� ��ü ���.
	B3DSprite_Translation(&_sprWIBody, 0, 
		g_B3Mode.Height - (_sprWIBody.Height+2), 1.0f);

	B3DSprite_Draw(&_sprWIBody, NO_, YES_, 0.5f);


	//�̻��� ������ ���.
	for(DWORD i=0; i<MissileMax; i++)
	{
		if(pMissile[i] == NULL) continue;
		if(pMissile[i]->IsReleased()) continue;

		float scaleX = _sprMissileIcon.Width*0.3f;
		float scaleY = _sprMissileIcon.Height*0.6f;

		float posx = 20.0f + i*(scaleX + 4);
		float posy = g_B3Mode.Height - (scaleY+20.0f);

		B3DSprite_Scaling(&_sprMissileIcon, scaleX, scaleY );		 
		B3DSprite_Translation(&_sprMissileIcon, posx , posy, 1.0f);
		B3DSprite_Draw(&_sprMissileIcon, NO_, YES_, 0.7f);

	}

}


///////////////////////////////////////////////////////////////////////////
// 
void GP::WeaponIndicatorRelease()
{
	//���� ǥ�ñ� ����. 2004.07.02._pMeshADI
	B3DSprite_Release(&_sprMissileIcon);	 
	B3DSprite_Release(&_sprWIBody);	 
}







/////////////////////////////////////////////////////////////////////////
//
// �ڼ��� ADI (Attitude Director Indicator) ����._pMeshADI
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//_pMeshADI
void GP::ADILoad()
{
	//'ADI' �� �޽� �ε�._pMeshADI
	_pMeshADI = new CB3Mesh(g_B3pDevice, "�����_ADI2.X");
	ASSERT(_pMeshADI != NULL);
	D3DXMatrixIdentity(&_mTMADI);

	B3DSprite_Load(&_sprADIBody, "�����_������׵θ�.bmp");		//����� �׵θ�.

}

/////////////////////////////////////////////////////////////////////////
//_pMeshADI
void GP::ADIMove(CPlane* pHero)
{
	//���ΰ��� �������� 'ADI' �� �ڼ� ���(Rolling Attitude Sphere) �� 
	//�����Ѵ�. -> ���ΰ� ȸ�� ����� ����ķ� ó��. 
	MATRIX mRot = pHero->GetRotMtx(); 
	D3DXMatrixInverse(&mRot, NULL, &mRot);

	float scale = 30.f;
	MATRIX mScale; 
	D3DXMatrixScaling(&mScale, scale, scale, scale); 

	MATRIX mTrans;
	D3DXMatrixTranslation(&mTrans, 
		(float)g_B3Mode.Width - (scale*2 + 5), 
		(float)g_B3Mode.Height - (scale*2 + 5), 
		30.0f);


	//�ڼ� ����� TM ó��.
	_mTMADI = mScale * mRot * mTrans;


	//�ڼ��� �׵θ��� TM ó��.
	B3DSprite_Scaling(&_sprADIBody, _sprADIBody.Width*2, _sprADIBody.Height*2); 
	B3DSprite_Translation(&_sprADIBody, 
		g_B3Mode.Width - (_sprADIBody.Width*2+1), 
		g_B3Mode.Height - (_sprADIBody.Height*2+1), 
		1.0f);



}



/////////////////////////////////////////////////////////////////////////
// Attitude Director Indicator ���. _pMeshADI
//
void GP::ADIDraw()
{
	//------------------------------------------------------------------------
	// ��ȯ ��� ó��.
	//------------------------------------------------------------------------
	//���� �������� ����.
	B3X_2DOn();
 	
	g_B3pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_B3pDevice->SetTransform(D3DTS_WORLD, &_mTMADI);


	//------------------------------------------------------------------------
	// ������ �ɼ�...
	//------------------------------------------------------------------------
	// ����Ʈ ON(�ɼ�..)
	/*D3DLIGHT9	light;		//���� ��ü.
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = COLOR(1, 1, 1, 1); 
	light.Direction = VECTOR3(1.0f, -1.0f, 0.0f); 
    g_B3pDevice->SetLight(0, &light);		//0�� ī�޶� ����.
	g_B3pDevice->LightEnable(0, TRUE);		//0�� ī�޶� ON!!
 	g_B3pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_B3pDevice->SetRenderState(D3DRS_AMBIENT, 0x00808080);
	*/
	// �ø� Off
	//g_B3pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	// Z-Buffer On!!
	g_B3pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);



	//------------------------------------------------------------------------
	//'�ڼ� ���(Rolling Attitude Sphere)' ������..._pMeshADI
	//------------------------------------------------------------------------
	_pMeshADI->Render(g_B3pDevice);


	//------------------------------------------------------------------------
	// ����Ʈ Off.
	g_B3pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_B3pDevice->SetRenderState(D3DRS_AMBIENT, 0x00);  


	//------------------------------------------------------------------------
	//'ADI' ��ü (�׵θ�) ���._pMeshADI
	//------------------------------------------------------------------------
	g_B3pDevice->SetRenderState(D3DRS_LIGHTING, FALSE); 
	B3DSprite_Draw(&_sprADIBody);


	//------------------------------------------------------------------------
	//������������ ��ȯ. ����.
	B3X_2DOff();
 
	//------------------------------------------------------------------------
	// ������ �ɼ� ����.
	//------------------------------------------------------------------------
 	 
	// �ø� On
	g_B3pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Z-Buffer On : 2004.06.17. ������ ON!!
	g_B3pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);


}





//////////////////////////////////////////////////////////////////////////////
//_pMeshADI
void GP::ADIRelease()
{	 
	B3DSprite_Release(&_sprADIBody);
 	SAFE_DELETE(_pMeshADI);
}




 

/**************** end of file "Instrument.cpp" *******************************/






