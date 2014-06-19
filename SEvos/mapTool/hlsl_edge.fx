// -------------------------------------------------------------
// 그림자 맵
// 
// Copyright (c) 2002-2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------
float4x4 mWVP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWVPB;		// 텍스처좌표계로의 투영
float4x4 mWLP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWLPB;		// 텍스처좌표계로의 투영
float4   vCol;		// 메시색
float4	 vLightDir;	// 광원방향
float    fId = 0;

float    fNear=1.0f;
float    fFar =7.0f;


// -------------------------------------------------------------
// 텍스처
// -------------------------------------------------------------
texture ShadowMap;
sampler ShadowMapSamp = sampler_state
{
    Texture = <ShadowMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
texture DecaleMap;
sampler DecaleMapSamp = sampler_state
{
    Texture = <DecaleMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};


// ------------------------------------------------------------
// 텍스처
// ------------------------------------------------------------
texture SrcTex;
sampler SrcSamp = sampler_state
{
    Texture = <SrcTex>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};



// -------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
	float4 Diffuse		: COLOR0;
	float4 Ambient		: COLOR1;
	float4 ShadowMapUV	: TEXCOORD0;
	float4 Depth		: TEXCOORD1;
	float4 DecaleTex	: TEXCOORD2;
};

// -------------------------------------------------------------
// 1패스:정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL,	         // 모델법선
      float2 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 좌표변환
	float4 pos = mul( Pos, mWLP );
	pos.z += 0.001f; // 실제 모델과 깊이값이 겹치기 때문에 뒤로 이동시킨다.
	
    // 위치좌표
    Out.Pos = pos;
    
    // 법선
    Out.Diffuse.w = (pos.z-fNear)/(fFar-fNear);  

    // 카메라좌표계에서의 깊이를 텍스처에 넣는다
    Out.ShadowMapUV = pos;

    return Out;
}


// -------------------------------------------------------------
// 1패스:픽셀셰이더
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{
    float4 Out;
	Out = float4(0, 0, 0, 1);
	Out.a = (In.Diffuse.w > 0.5f)? 1.f-In.Diffuse.w : In.Diffuse.w;	

    return Out;
}


// -------------------------------------------------------------
// 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_pass1(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL,	         // 모델법선
      float2 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
	
	// 좌표변환
    Out.Pos = mul(Pos, mWVP);
    
	Out.Diffuse = vCol * max( dot(vLightDir, Normal), 0);// 확산광
	Out.Ambient = 0.3f;							 // 환경광
	
	// 그림자 맵
	Out.ShadowMapUV = mul(Pos, mWLPB);

	// 비교를 위한 깊이값
	Out.Depth       = mul(Pos, mWLP);

	// 디컬 텍스처
	Out.DecaleTex   = mul(Pos, mWVPB);
	
    return Out;
}



// 2패스:픽셀셰이더(텍스처있음)
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT In) : COLOR
{
    float4 Color;
	float  shadow = tex2Dproj( ShadowMapSamp, In.ShadowMapUV ).x;
	float4 decale = tex2Dproj( DecaleMapSamp, In.DecaleTex );

//  Color = In.Ambient
//		 + ((shadow * In.Depth.w < In.Depth.z-0.03f) ? 0 : In.Diffuse);
//	Color = 0.3f
//		 + ((shadow * In.Depth.w < In.Depth.z-0.03f) ? 0 : In.Diffuse);		 

//	Color = 0.3f + ((shadow < In.Depth.z -0.33f) ? 0 : 0.7f);
//	Color = 1;

	Color = 0.3f + ((shadow < 1) ? 0 : 0.7f);
	
	return Color * decale;
}



// -------------------------------------------------------------
// 2패스:픽셀셰이더(텍스처없음)
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT In) : COLOR
{   
    float4 Color = 0;
	float  shadow = tex2Dproj( ShadowMapSamp, In.ShadowMapUV ).x;
    
    Color = In.Ambient
	 + ((shadow * In.Depth.w < In.Depth.z-0.03f) ? 0 : In.Diffuse);

    return Color;
}  

PixelShader PS_ZEdge = asm
{
    ps_1_1
    
	tex t0	// 현재프레임(좌측상단)
	tex t1	// 현재프레임(우측하단)
	tex t2	// 현재프레임(좌측하단)
	tex t3	// 현재프레임(우측상단)
	
	// 인수로 t#레지스터를 2개 사용할수 없으므로 일단 복사
	mov r0, t0
	mov r1, t2
	
	add_x2     r0,   r0,  -t1	;       r0                  r1
	add_x2     r1,   r1,  -t3	; 4*(t0.a-t1.a)        4*(t2.a-t3.a)
	mul_x2		r0,   r0,   r0	;
	mul_x2		r1,   r1,   r1	; 16*(t0-t1깊이)^2,  16*(t2-t3깊이)^2)
	add_x2     r0,   r0,   r1	; r0.a = 64((t0-t0깊이)^2+(t3-t1깊이)^2)
	mov        r0,   1-r0.a		; r0.a = (1-64((t0-t0깊이)^2+(t3-t1깊이)^2))
};

	
// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{
    pass P0// 그림자맵 생성
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_2_0 PS_pass0();
    }
    
    pass P1// 알파블렌딩
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_2_0 PS_pass1();
    }    
    
    pass P2 // 엣지
    {
        // 셰이더
        PixelShader  = <PS_ZEdge>;
		// 텍스처
		Sampler[0] = (SrcSamp);
		Sampler[1] = (SrcSamp);
		Sampler[2] = (SrcSamp);
		Sampler[3] = (SrcSamp);
	}
    
}
