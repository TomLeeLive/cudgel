Texture2D g_txDiffuse: register (t0);
Texture2D g_txTex0: register (t1);
Texture2D g_txTex1: register (t2);
Texture2D g_txTex2: register (t3);
Texture2D g_txTex3: register (t4);
Texture2D g_txTex4: register (t5);

SamplerState samLinear: register (s0);
cbuffer cb0
{
	float4x4	g_matWorld		: packoffset(c0);
	float4x4	g_matView		: packoffset(c4);
	float4x4	g_matProj		: packoffset(c8);
	float4  g_MeshColor     : packoffset(c12);
};
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 p : POSITION;
	float4 c : COLOR;
	float2 t : TEXCOORD;
};
struct VS_OUTPUT
{
	float4 p : SV_POSITION;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
// float4  정점을 반환해야 한다.
VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT output;
	output.p = mul(vIn.p, g_matWorld);
	output.p = mul(output.p, g_matView);
	output.p = mul(output.p, g_matProj);
	output.t = vIn.t;
	output.c = vIn.c;// *g_MeshColor;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT vIn) : SV_Target
{
	float4 cColorA = g_txDiffuse.Sample(samLinear, vIn.t)*vIn.c;
	return cColorA;
}

float4 blendps(VS_OUTPUT vIn) : SV_Target
{
	float4 cColorA = g_txDiffuse.Sample(samLinear, vIn.t);
	float4 cColorB = g_txTex0.Sample(samLinear, vIn.t);
	//if (cColorA.a < 0.1) discard;
	float4 cFinal;
	cFinal.x = cColorB.x * (1 - cColorA.w) + cColorA.x * cColorA.w;
	cFinal.y = cColorB.y * (1 - cColorA.w) + cColorA.y * cColorA.w;
	cFinal.z = cColorB.z * (1 - cColorA.w) + cColorA.z * cColorA.w;
	cFinal.w = 1;// g_MeshColor.w;
	return cFinal;
}

float4 PS_Color(VS_OUTPUT vIn) : SV_Target
{
	float2 vTex = vIn.t;

	float4 cDiffuse = g_txDiffuse.Sample(samLinear, vTex);
	float4 cMask = g_txTex0.Sample(samLinear, vIn.t);

	//vTex.y = g_MeshColor.w;
	float4 cColor1 = g_txTex1.Sample(samLinear, vTex);// a=cColor0.r
	float4 cColor2 = g_txTex2.Sample(samLinear, vTex);// a=cColor1.g
	float4 cColor3 = g_txTex3.Sample(samLinear, vTex);// a=cColor1.b
	float4 cColor4 = g_txTex4.Sample(samLinear, vTex);// a=cColor1.a

	float4 cFinal = cColor1 * cMask.r;
	//cFinal += cColor2*cMask.g;
	//cFinal += cColor3*cMask.b;
	//cFinal += cColor4*cMask.a;
	//cFinal.x = cColorB.x * (1 - cColorA.w) + cColorA.x * cColorA.w;
	//cFinal.y = cColorB.y * (1 - cColorA.w) + cColorA.y * cColorA.w;
	//cFinal.z = cColorB.z * (1 - cColorA.w) + cColorA.z * cColorA.w;
	//cFinal.w = g_MeshColor.w;
	return cFinal;
}