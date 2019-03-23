// SkyBox.cpp: implementation of the CSkyBox class.
//
//
////////////////////////////////////////////////////////////////////////////// 

#include "../../Engine/Ext/B3Yena_Ext.h"	//DLL 버전 엔진 필수헤더. 
#include "SkyBox.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CSkyBox::CSkyBox( 
		LPDEVICE	pDev,				//디바이스 포인터.
		CPlane*		pHero,				//주인공 정보 포인터.
		CCamera**	ppCamera,			//카메라 포인터.
		
		SPRDATA		pSpr[SBF_MAX_],		//스카이 박스용 텍스쳐 저장용 스프라이트 배열.
		VECTOR3		vPos/*=VECTOR3(0, 0, 0)*/,	//'월드 위치'
		VECTOR3		vScale /*=VECTOR3(512, 512, 512)*/		//박스 크기 : 약 512 정도.
		)
:CUnit(vPos, VECTOR3(0, 0, 0), vScale, 
	   VECTOR3(0, 0, 0), VECTOR3(0, 0, 0), 0.0f, NULL, NULL, pDev)	 
,m_pHero(pHero), m_ppCamera(ppCamera)
{ 
	//텍스쳐 정보 복사.
	memcpy(m_Spr, pSpr, sizeof(SPRDATA)*SBF_MAX_);		
	
	//텍스쳐 행렬 클리어.
	//for(int i=SBF_TOP_; i<SBF_MAX_; i++)
	//{
	//	D3DXMatrixIdentity(&m_SprTM[i]);
	//}


	//카메라 의 '눈'이 스카이 박스의 '중심'이 된다.
	m_vPos = (*m_ppCamera)->GetEye(); 


	Init();
}


//////////////////////////////////////////////////////////////////////////////
//
CSkyBox::~CSkyBox()
{
	Release();
}



//////////////////////////////////////////////////////////////////////////////
//
// 내부 메소드 구현.
//
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//
//  하늘상자용 메쉬 제작 : 크기  1x1x1 짜리 메쉬.
//  UV 좌표는 '내부' 에서 바라볼때의 기준으로 처리된다.
//  정면은 -Z  우측은 -X 상부는 +Y
HRESULT CSkyBox::_CreateVB()
{ 

	SBOX_VTX	vertices[] = {
	
	// 상자의 내부->정면(-Z) 를 보는 기준으로 정점/uv 배정.
	// 이때, 우측은 -x 상부는 +Y 가 된다. 주의
	//0번 면 : 정면.
	{  0.5f, 0.5f, -0.5f,   0.0f, 0.0f },		//v0	- 좌측 상단.
	{ -0.5f, 0.5f, -0.5f,   1.0f, 0.0f },		//v1	- 우측 상단.
	{  0.5f,-0.5f, -0.5f,   0.0f, 1.0f },		//v2	- 좌측 하단.

	{  0.5f,-0.5f, -0.5f,   0.0f, 1.0f },		//v3	- 좌측 하단.
	{ -0.5f, 0.5f, -0.5f,   1.0f, 0.0f },		//v4	- 우측 상단.
	{ -0.5f,-0.5f, -0.5f,   1.0f, 1.0f },		//v5	- 우측 하단.


	//1번 면 : 우측면
	{  -0.5f, 0.5f, -0.5f,   0.0f, 0.0f },		//v6	 
	{  -0.5f, 0.5f,  0.5f,   1.0f, 0.0f },		//v7	 
	{  -0.5f,-0.5f, -0.5f,   0.0f, 1.0f },		//v8	 

	{  -0.5f,-0.5f, -0.5f,   0.0f, 1.0f },		//v9	 
	{  -0.5f, 0.5f,  0.5f,   1.0f, 0.0f },		//v10	 
	{  -0.5f,-0.5f,  0.5f,   1.0f, 1.0f },		//v11	 



	//2번 면 : 뒷면
	{ -0.5f, 0.5f, 0.5f,   0.0f, 0.0f },		//v12 
	{  0.5f, 0.5f, 0.5f,   1.0f, 0.0f },		//v13 
	{ -0.5f,-0.5f, 0.5f,   0.0f, 1.0f },		//v14 

	{ -0.5f,-0.5f, 0.5f,   0.0f, 1.0f },		//v15 
	{  0.5f, 0.5f, 0.5f,   1.0f, 0.0f },		//v16 
	{  0.5f,-0.5f, 0.5f,   1.0f, 1.0f },		//v17 


	//3번 면 : 좌측면
	{ 0.5f, 0.5f,  0.5f,   0.0f, 0.0f },		//v18 
	{ 0.5f, 0.5f, -0.5f,   1.0f, 0.0f },		//v19 
	{ 0.5f,-0.5f,  0.5f,   0.0f, 1.0f },		//v20 

	{ 0.5f,-0.5f,  0.5f,   0.0f, 1.0f },		//v21 
	{ 0.5f, 0.5f, -0.5f,   1.0f, 0.0f },		//v22 
	{ 0.5f,-0.5f, -0.5f,   1.0f, 1.0f },		//v23 


	//4번 면 : 윗 뚜껑.: 정면->위를 바라보는 기준..
	{  0.5f, 0.5f,  0.5f,   0.0f, 0.0f },		//v24 
	{ -0.5f, 0.5f,  0.5f,   1.0f, 0.0f },		//v25 
	{  0.5f, 0.5f, -0.5f,   0.0f, 1.0f },		//v26 

	{  0.5f, 0.5f, -0.5f,   0.0f, 1.0f },		//v27 
	{ -0.5f, 0.5f,  0.5f,   1.0f, 0.0f },		//v28 
 	{ -0.5f, 0.5f, -0.5f,   1.0f, 1.0f },		//v29 

	//5번 면 : 아래 뚜껑.정면->아래를 바라보는 기준.
	{  0.5f, -0.5f, -0.5f,   0.0f, 0.0f },		//v30 
	{ -0.5f, -0.5f, -0.5f,   1.0f, 0.0f },		//v31 
	{  0.5f, -0.5f,  0.5f,   0.0f, 1.0f },		//v32 

	{  0.5f, -0.5f,  0.5f,   0.0f, 1.0f },		//v33 
	{ -0.5f, -0.5f, -0.5f,   1.0f, 0.0f },		//v34 
	{ -0.5f, -0.5f,  0.5f,   1.0f, 1.0f },		//v35 
	};
 


	//
	// 정점 버퍼 생성.
	//
	if(FAILED(m_pDev->CreateVertexBuffer(sizeof(vertices),  
		0, FVF_SKYBOX, D3DPOOL_MANAGED, &m_pVBSBox, NULL)))
	{
		return E_FAIL;
	}

	//버퍼 채우기.
	VOID*	pVB = NULL;

	if(FAILED(m_pVBSBox->Lock(0, NULL, (void**)&pVB, 0 )))
	{
		return E_FAIL;
	}

	memcpy(pVB, vertices, sizeof(vertices));

	m_pVBSBox->Unlock();

	return S_OK;

}


//////////////////////////////////////////////////////////////////////////////
//
// 메소드 구현.
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
int CSkyBox::Init()
{
	_CreateVB();

	return TRUE;
}




//////////////////////////////////////////////////////////////////////////////
//
void CSkyBox::Release()
{
	//N/A
	SAFE_RELEASE(m_pVBSBox);		//@Update
}







//////////////////////////////////////////////////////////////////////////////
//
int CSkyBox::Move(float dTime /* = 0 */)
{
	//카메라 의 '눈'이 스카이 박스의 '중심'이 된다.
	m_vPos = (*m_ppCamera)->GetEye(); 

	D3DXMatrixScaling(&m_mScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixTranslation(&m_mTranslation, m_vPos.x, m_vPos.y, m_vPos.z);
	m_mTM = m_mScale * m_mTranslation;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//
int CSkyBox::Draw(float dTime /* = 0 */)
{
	//이전 옵션 상태 얻기.
	//..... 

	//텍스쳐 필터링 옵션 설정.

	//옵션 설정.
	m_pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);	//알파 연산 끄기.
	m_pDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//안개..등의 옵션도 끄기...
	m_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);			//컬링 가능.
	m_pDev->SetRenderState(D3DRS_LIGHTING, FALSE); 
	m_pDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);				// Z-Buffer Write 끄기. 
	m_pDev->SetRenderState(D3DRS_ZENABLE, FALSE);					// Z-Buffer 끄기.

	//셈플링 옵션 설정.
	m_pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	//텍스쳐 필터링.
	m_pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
 
	//Clamp Address 설정. : 텍스쳐에 따라 설정 필요.
	m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	

	//정점 스트림 설정.
	//B3DSprite_SetStream();		 
	m_pDev->SetStreamSource(0, m_pVBSBox, 0, sizeof(SBOX_VTX));
	m_pDev->SetTransform(D3DTS_WORLD, &m_mTM); 
	m_pDev->SetFVF(FVF_SKYBOX);

	//6개의 면을 렌더링한다.
	for(int i=SBF_FRONT_; i<SBF_MAX_; i++)
	{
		//텍스쳐 셋팅... 
  		m_pDev->SetTexture(0, m_Spr[i].pTexture);
 		
		//렌더링... 
 		m_pDev->DrawPrimitive(D3DPT_TRIANGLELIST, i*6, 2 ); 
	}

 	//옵션 복구.
	m_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	//컬링 가능.
	m_pDev->SetRenderState(D3DRS_LIGHTING, TRUE); 
	m_pDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);	//-> 필요시 설정하도록.. 
	m_pDev->SetRenderState(D3DRS_ZENABLE, TRUE);		// Z-Buffer 끄기.


	//셈플링 옵션 설정.: 엔진의 것으로 복구.
	m_pDev->SetSamplerState(0, D3DSAMP_MINFILTER, B3D::g_B3TexFilterMin);
	m_pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, B3D::g_B3TexFilterMag);
	m_pDev->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, B3D::g_B3TexFilterMaxAnisotropy);

	//Clamp Address 설정.
	m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
 
	return TRUE;
}







/****************** end of file "SkyBox.cpp" ********************************/