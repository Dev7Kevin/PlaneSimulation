//
// Instrument.cpp :  계기류 소스 파일.
//
//
//////////////////////////////////////////////////////////////////////////////


#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL 버전 엔진 필수헤더. 


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
	// 자세계 - ADI (Attitude Director Indicator) 관련. 2004.07.02. 
	//
	CB3Mesh*	_pMeshADI = NULL;
	MATRIX		_mTMADI;
	SPRDATA		_sprADIBody;
 

	//
	// 무장 표시기 - WI (Weapon Indicator) 관련. 2004.07.02.  
	//
	SPRDATA	_sprWIBody;
	SPRDATA _sprMissileIcon;

	
	//
	// 2D 레이다.
	//
	SPRDATA _sprRWRBody;
	SPRDATA _sprRWRDot;

	typedef list<CEnemy*>::iterator EmyItor_;
	extern list<CEnemy*> _EnemyList;				//적기 리스트.
	extern	CHero*	_pHero;

	typedef list<VECTOR3>::iterator RWREmyItor_;
	list<VECTOR3> _RWREmyList;						//스캔된 적기 리스트.(위치만 필요)



};








///////////////////////////////////////////////////////////////////////////
// 
// 2D Radar 관련.	
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// 
void GP::RWRLoad() 	
{

	B3DSprite_Load(&_sprRWRBody, "RWR_body.bmp");
	B3DSprite_Scaling(&_sprRWRBody, 
		_sprRWRBody.Width, 
		_sprRWRBody.Height);		//실제 사이즈 만큼 스케일링.


	B3DSprite_Load(&_sprRWRDot, "RWR_Dot.bmp");
	B3DSprite_Scaling(&_sprRWRDot, 
		_sprRWRDot.Width, 
		_sprRWRDot.Height);		//실제 사이즈 만큼 스케일링.


}

///////////////////////////////////////////////////////////////////////////
// 
void GP::RWRUpdate(float dTime /* = 0.0f */)	
{
	//0.스캔 리스트 클리어.
	_RWREmyList.clear();


	//1.주인공의 월드 위치를 기준으로 일정 반경(5km) 적기를 검색
	//  리스트에 등록한다...
	const float RWR_RADIUS = 5000.0f;

	for(EmyItor_ i= _EnemyList.begin(); i != _EnemyList.end(); i++)
	{
		CEnemy* pEmy = *i;
		if(pEmy->CUnit::CheckCollision(_pHero, RWR_RADIUS) == CUnit::COLL_YES_)
		{
			//충돌..즉, 레디어 범위안에 들어 왔으므로, 적 유닛의 위치를
			//리스트에 등록한다...
			_RWREmyList.push_back(pEmy->GetPos());
		}
	}


	//2.스캔된 넘들의 위치를 주인공 중심으로 재 배치한다.
	//
	MATRIX mHeroView;

	//주인공 월드 위치를 중심 기준시.
	/*VECTOR3 vLookAt = _pHero->GetModelNView()*10.0f;
	vLookAt.y = 0.0f;
	vLookAt += _pHero->GetPos();

	D3DXMatrixLookAtLH(&mHeroView, 
	&_pHero->GetPos(), 
	&vLookAt, 
	&VECTOR3(0, 1, 0)	
	);*/


	//주인공 모델 공간의 중심을 기준 변환행렬 생성..
	D3DXMatrixLookAtLH(&mHeroView, 
		&_pHero->GetPos(), 
		&(_pHero->GetPos() + _pHero->GetNView()*10.0f), 
		&_pHero->GetUp()  //&VECTOR3(0, 1, 0)	
		);

	float ratio = (_sprRWRBody.Width * 0.5f) / RWR_RADIUS;

	for(RWREmyItor_ i= _RWREmyList.begin();  i!= _RWREmyList.end(); i++)
	{		 
		VECTOR3 vPos = (*i);

		//적기의 주인공 중심으로 판정, 즉 주인공 공간으로 투영한다.
		D3DXVec3TransformCoord(&vPos, &vPos, &mHeroView);

		//RWR 이미지의 '범위' 안에 들어가도록, 좌표를 재계산한다.
		vPos *= ratio;
		(*i) = vPos;			//보정된 좌표 저장.				 
	}


}






///////////////////////////////////////////////////////////////////////////
// 
void GP::RWRDraw()	
{
	//무장 표시기 몸체 출력.
	VECTOR3 vDrawPos = VECTOR3( _sprWIBody.Width + 3, 
		g_B3Mode.Height - (_sprRWRBody.Height+2), 1.0f);
	B3DSprite_Translation(&_sprRWRBody, vDrawPos.x, vDrawPos.y, vDrawPos.z);
	B3DSprite_Draw(&_sprRWRBody, NO_, YES_, 0.5f);



	//레이더에 스캔된 적 기체 출력... 
	//뷰포트 처리를 통해 더 심플하게 할 수 있겠으나, 여기서는 걍 계산한다.
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
// 무장 표시기 (Weapon Indicator)  관련.
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// 
// 미사일 아이콘. : '계기판'의 무장 표시기로 사용할 것이다. 
// 2D 직교 투영으로 사용할것임. 
// 2004.07.02._pMeshADI 
//
void GP::WeaponIndicatorLoad()
{
	B3DSprite_Load(&_sprMissileIcon, "미사일_아이콘.bmp");

	B3DSprite_Load(&_sprWIBody, "미사일_표시기_몸체.bmp");
 	B3DSprite_Scaling(&_sprWIBody, 
					_sprWIBody.Width, 
					_sprWIBody.Height);		//실제 사이즈 만큼 스케일링.


}

///////////////////////////////////////////////////////////////////////////
// 
//  무장 표시기 출력._pMeshADI
//
void GP::WeaponIndicatorDraw(CMissile* pMissile[], DWORD MissileMax)
{
	//무장 표시기 몸체 출력.
	B3DSprite_Translation(&_sprWIBody, 0, 
		g_B3Mode.Height - (_sprWIBody.Height+2), 1.0f);

	B3DSprite_Draw(&_sprWIBody, NO_, YES_, 0.5f);


	//미사일 아이콘 출력.
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
	//무장 표시기 관련. 2004.07.02._pMeshADI
	B3DSprite_Release(&_sprMissileIcon);	 
	B3DSprite_Release(&_sprWIBody);	 
}







/////////////////////////////////////////////////////////////////////////
//
// 자세계 ADI (Attitude Director Indicator) 관련._pMeshADI
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//_pMeshADI
void GP::ADILoad()
{
	//'ADI' 용 메쉬 로드._pMeshADI
	_pMeshADI = new CB3Mesh(g_B3pDevice, "계기판_ADI2.X");
	ASSERT(_pMeshADI != NULL);
	D3DXMatrixIdentity(&_mTMADI);

	B3DSprite_Load(&_sprADIBody, "계기판_수평계테두리.bmp");		//계기판 테두리.

}

/////////////////////////////////////////////////////////////////////////
//_pMeshADI
void GP::ADIMove(CPlane* pHero)
{
	//주인공의 움직임을 'ADI' 의 자세 제어구(Rolling Attitude Sphere) 에 
	//적용한다. -> 주인공 회전 행렬의 역행렬로 처리. 
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


	//자세 제어구의 TM 처리.
	_mTMADI = mScale * mRot * mTrans;


	//자세계 테두리의 TM 처리.
	B3DSprite_Scaling(&_sprADIBody, _sprADIBody.Width*2, _sprADIBody.Height*2); 
	B3DSprite_Translation(&_sprADIBody, 
		g_B3Mode.Width - (_sprADIBody.Width*2+1), 
		g_B3Mode.Height - (_sprADIBody.Height*2+1), 
		1.0f);



}



/////////////////////////////////////////////////////////////////////////
// Attitude Director Indicator 출력. _pMeshADI
//
void GP::ADIDraw()
{
	//------------------------------------------------------------------------
	// 변환 행렬 처리.
	//------------------------------------------------------------------------
	//직교 투영으로 설정.
	B3X_2DOn();
 	
	g_B3pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_B3pDevice->SetTransform(D3DTS_WORLD, &_mTMADI);


	//------------------------------------------------------------------------
	// 렌더링 옵션...
	//------------------------------------------------------------------------
	// 라이트 ON(옵션..)
	/*D3DLIGHT9	light;		//광원 객체.
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = COLOR(1, 1, 1, 1); 
	light.Direction = VECTOR3(1.0f, -1.0f, 0.0f); 
    g_B3pDevice->SetLight(0, &light);		//0번 카메라 설정.
	g_B3pDevice->LightEnable(0, TRUE);		//0번 카메라 ON!!
 	g_B3pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_B3pDevice->SetRenderState(D3DRS_AMBIENT, 0x00808080);
	*/
	// 컬링 Off
	//g_B3pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	// Z-Buffer On!!
	g_B3pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);



	//------------------------------------------------------------------------
	//'자세 제어구(Rolling Attitude Sphere)' 렌더링..._pMeshADI
	//------------------------------------------------------------------------
	_pMeshADI->Render(g_B3pDevice);


	//------------------------------------------------------------------------
	// 라이트 Off.
	g_B3pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_B3pDevice->SetRenderState(D3DRS_AMBIENT, 0x00);  


	//------------------------------------------------------------------------
	//'ADI' 몸체 (테두리) 출력._pMeshADI
	//------------------------------------------------------------------------
	g_B3pDevice->SetRenderState(D3DRS_LIGHTING, FALSE); 
	B3DSprite_Draw(&_sprADIBody);


	//------------------------------------------------------------------------
	//원근투영으로 전환. 설정.
	B3X_2DOff();
 
	//------------------------------------------------------------------------
	// 렌더링 옵션 복구.
	//------------------------------------------------------------------------
 	 
	// 컬링 On
	g_B3pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Z-Buffer On : 2004.06.17. 무조건 ON!!
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






