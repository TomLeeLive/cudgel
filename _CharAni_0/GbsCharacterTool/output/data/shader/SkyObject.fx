//--------------------------------------------------------------------------------------
// 전역 변수 선언
//--------------------------------------------------------------------------------------
cbuffer cbPerObject : register(b0)
{
	matrix		g_matWorld	: packoffset(c0);
	matrix		g_matView	: packoffset(c4);
};
cbuffer cbPerScene : register(b1)
{
	matrix		g_matProj	: packoffset(c0);
};
cbuffer cbPerSkyObject : register(b2)
{
	matrix		g_matViewCube[6];	: packoffset(c0);
};
//--------------------------------------------------------------------------------------
// 환경맵 (Cube Texture) & Diffuse Texture
//--------------------------------------------------------------------------------------
Texture2D		g_txDiffuse: register(t0);
TextureCube		g_txCubeMap: register(t1);
SamplerState	g_samWrap : register(s0);
SamplerState	g_samCubeClamp: register(s1);

struct PNCT_VERTEX
{
    float4 p			: POSITION;
    float3 n			: NORMAL;
    float4 c			: COLOR0;
    float2 t			: TEXCOORD0;
};
struct P4T2_PS_INPUT
{
    float4 p			: SV_POSITION;  
    float3 v			: TEXCOORD0;
};
struct GS_Input {
	float4 p : SV_POSITION;
	float3 v : TEXCOORD0;
};
struct PS_Input {
	float4 p : SV_POSITION;
	float3 v : TEXCOORD0;
	uint RTIndex : SV_RenderTargetArrayIndex;
};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
P4T2_PS_INPUT OBJECT_VS( PNCT_VERTEX input )
{
    P4T2_PS_INPUT output = (P4T2_PS_INPUT)0;
    float4 vWorldPos = mul( input.p,  g_matWorld );
    float4 vViewPos = mul( vWorldPos, g_matView );
    float4 vProjPos	= mul( vViewPos, g_matProj );
   
    // Set z = w so that z/w = 1 (i.e., skydome always on far plane). 
    output.p = vProjPos.xyww;
    output.v = input.p.xyz; 
    return output;
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 OBJECT_PS( P4T2_PS_INPUT input) : SV_Target
{		
	return g_txCubeMap.Sample(g_samCubeClamp, input.v);
}


GS_Input VS_Cube( PNCT_VERTEX input )
{
    GS_Input output = (GS_Input)0;
    float4 vWorldPos = mul( input.p,  g_matWorld );	
    output.p = vWorldPos;
	// 정점 회전시는 텍스처 좌표를 고정한다.
	// 또는 정점을 초기 위치로 하고 텍스처 좌표를 갱신하여도 된다.
    output.v = normalize(input.p.xyz); 	
    return output;
}
[maxvertexcount(24)]
void GS_Cube(	triangle GS_Input input[3],
				inout TriangleStream<PS_Input> cubeMapStream )
{
	// 스크린좌표를 계산하여 출력함.
	for ( int f=0; f<6; f++ ) 
	{
		PS_Input output;
		output.RTIndex = f;
		for ( int v=0; v<3; v++ ) 
		{
			output.p = mul ( mul( input[v].p, g_matViewCube[f]), g_matProj);
			output.p = output.p.xyww;
			output.v = input[v].v; 
			cubeMapStream.Append( output );
		}
		cubeMapStream.RestartStrip();
	}
}

float4 PS_Cube( PS_Input input): SV_Target
{
	float4 final = g_txCubeMap.Sample(g_samWrap, input.v);
	return final;
}
//
//technique11 RenderCube
//{
//	pass P0
//	{
//		SetVertexShader( CompileShader( vs_5_0, VS_Cube() ) );
//		SetGeometryShader(CompileShader( gs_5_0, GS_Cube() ) );
//		SetPixelShader( CompileShader( ps_5_0, PS_Cube() ) );
//
//		SetRasterizerState( NoneCullSolid );       
//        // z버퍼에 1값을 세팅한다.
//        SetDepthStencilState( AlwaysEnableDepth, 0 );
//	}
// }
//
//technique11 ObjectRender
//{
//    pass P0
//    {
//        SetVertexShader( CompileShader( vs_5_0, OBJECT_VS() ) );
//        SetGeometryShader( NULL );
//        SetPixelShader( CompileShader( ps_5_0, OBJECT_PS() ) );
//        
//        SetRasterizerState( NoneCullSolid );       
//        // z버퍼에 1값을 세팅한다.
//        SetDepthStencilState( AlwaysEnableDepth, 0 );
//    }
//}

//SamplerState g_samCubeClamp
//{
//    Filter = ANISOTROPIC;
//    AddressU = Clamp;
//    AddressV = Clamp;
//};
//SamplerState textureSampler {
//	Filter = MIN_MAG_MIP_LINEAR;
//	AddressU = Wrap;
//	AddressV = Wrap;
//};
//RasterizerState NoneCullSolid
//{
//    CullMode = NONE;
//    FillMode = SOLID;
//};
//DepthStencilState AlwaysEnableDepth
//{
//    DepthEnable		= TRUE;
//    DepthWriteMask	= ALL;
//    DepthFunc		= Always;
//};
//-----------------------------------------------------------------------------
//정점 및 픽셀 쉐이더 입출력 버퍼
//-----------------------------------------------------------------------------