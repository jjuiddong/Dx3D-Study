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
//float4x4 mWLP;		// 로컬에서 투영공간으로의 좌표변환
//float4x4 mWLPB;		// 텍스처좌표계로의 투영
//float4   vCol;
float distNearFar = 1000.f;	// 투영공간에서 near - far 사이 거리

float4 vLightDir;	// 광원방향
float3 vEyePos;		// 카메라위치(로컬좌표계)

// -------------------------------------------------------------
// 텍스처
// -------------------------------------------------------------
texture AlphaMap;
sampler AlphaMapSamp = sampler_state
{
    Texture = <AlphaMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

// -------------------------------------------------------------
texture TerrainMap;
sampler TerrainMapSamp = sampler_state
{
    Texture = <TerrainMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

texture TileMap1;
sampler TileMapSamp1 = sampler_state
{
    Texture = <TileMap1>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

texture TileMap2;
sampler TileMapSamp2 = sampler_state
{
    Texture = <TileMap2>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

texture TileMap3;
sampler TileMapSamp3 = sampler_state
{
    Texture = <TileMap3>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

texture TileMap4;
sampler TileMapSamp4 = sampler_state
{
    Texture = <TileMap4>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

// ------------------------------------------------------------
// 텍스처
// ------------------------------------------------------------
texture BackGroundMap;
sampler BackGroundMapSamp = sampler_state
{
    Texture = <BackGroundMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
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
	float4 Color		: COLOR0;
	float2 Tex			: TEXCOORD0;
    float3 N			: TEXCOORD1;
    float3 Eye			: TEXCOORD2;	
    float3 Loc			: TEXCOORD3;
};

// -------------------------------------------------------------
// 0패스:정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL,	         // 모델법선
      float2 Tex    : TEXCOORD0	         // 텍스처좌표      
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터

    // 좌표변환
	float4 pos = mul( Pos, mWVP );

    // 위치좌표
    Out.Pos = pos;
    Out.Loc = Pos.xyz;

    // 텍스처 좌표
    Out.Tex = Tex;

	float amb = -vLightDir.w;	// 환경광의 강도
	float3 L = -vLightDir.xyz; // 로컬좌표계에서의 광원벡터
//	Out.Color = vColor * max(amb, dot(Normal, -vLightDir));
	Out.Color.a = max(amb, dot(Normal, -vLightDir.xyz));
	
	// 반영반사용 벡터
	Out.N   = Normal.xyz;
	Out.Eye = vEyePos - Pos.xyz;
    
    return Out;
}


// -------------------------------------------------------------
// 0패스:픽셀셰이더
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{
	float3 L = -vLightDir.xyz;
	float3 H = normalize(L + normalize(In.Eye));	// 하프벡터
	float3 N = normalize(In.N);

	float4 Out;
	float4 base = tex2D( TerrainMapSamp, In.Tex );
	float4 alpha = tex2D( AlphaMapSamp, In.Tex );
	float4 tile1 = tex2D( TileMapSamp1, In.Tex );
	float4 tile2 = tex2D( TileMapSamp2, In.Tex );
	float4 tile3 = tex2D( TileMapSamp3, In.Tex );
	float4 tile4 = tex2D( TileMapSamp4, In.Tex );
	
	Out = base;
	Out = (Out * (1.f - alpha.a)) + (tile1 * alpha.a);
	Out = (Out * (1.f - alpha.r)) + (tile2 * alpha.r);
	Out = (Out * (1.f - alpha.g)) + (tile3 * alpha.g);
	Out = (Out * (1.f - alpha.b)) + (tile4 * alpha.b);
	
	Out = In.Color.a * Out;	// 확산광+환경광
	//	 + pow(max(0,dot(N, H)), 10);	// 반영반사광
	
// 	float depthAlpha = 1f;
// 	float len = max(0, -10 - In.Pos.y);
// 	len = saturate(len * depthAlpha);
// 	len = rsqrt(len);
// 	len = saturate( Rcp(len) );
// 
// 	float4 absorbLay(1, 1, 1, 1);	// 빛의 흡수를 위한 값 (적, 녹, 청)
// 	float4 absorbLay * len;
	Out.a = 1;	

    return Out;
}



// -------------------------------------------------------------
// 1패스:정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_pass1(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL,	         // 모델법선
      float2 Tex    : TEXCOORD0	         // 텍스처좌표      
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터

    // 좌표변환
	float4 pos = mul( Pos, mWVP );

    // 위치좌표
    Out.Pos = pos;
    Out.Loc.z = pos.z / distNearFar;

    // 텍스처 좌표
    Out.Tex = Tex;

//	float amb = -vLightDir.w;	// 환경광의 강도
//	float3 L = -vLightDir.xyz; // 로컬좌표계	에서의 광원벡터
//	Out.Color = vColor * max(amb, dot(Normal, -vLightDir));
//	Out.Color.a = max(amb, dot(Normal, -vLightDir.xyz));
	
	// 반영반사용 벡터
//	Out.N   = Normal.xyz;
//	Out.Eye = vEyePos - Pos.xyz;

    return Out;
}


// -------------------------------------------------------------
// 1패스:픽셀셰이더
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT In) : COLOR
{
	float4 Out;
//	Out = In.Loc.z;
//	Out = float4(1,0,1,1);
//	float4 base = tex2D( TerrainMapSamp, In.Tex );
//	Out = base;	

	Out = In.Loc.z;
	Out.a = 1;

    return Out;
}


// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{
    pass P0// 지형 출력
    {
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_2_0 PS_pass0();
//	    Sampler[0] = (TerrainMapSamp);
    }

    pass P1// 깊이값 저장
    {
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_2_0 PS_pass1();
    }
    
}

