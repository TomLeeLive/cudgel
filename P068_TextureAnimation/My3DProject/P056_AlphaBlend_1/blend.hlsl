Texture2D g_txDiffuse: register (t0);
Texture2D g_txFront: register (t1);

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
	//float4 cColorA = g_txDiffuse.Sample(samLinear, vIn.t);
	//float4 cColorB = g_txFront.Sample(samLinear, vIn.t);
	////if (cColorA.a < 0.1) discard;
	//float4 cFinal;
	//cFinal.r =  cColorB.r * (1 - cColorA.a ) + cColorA.r * cColorA.a;
	//cFinal.g =  cColorB.g * ( 1- cColorA.a ) + cColorA.g * cColorA.a;
	//cFinal.b =  cColorB.b * ( 1- cColorA.a ) + cColorA.b * cColorA.a;
	//cFinal.a = 1;
	//return float4(1-cFinal.r, 1-cFinal.g, 1-cFinal.b,1);

	float4 final;
	//final.r = vIn.r;
	//final.g = vIn.g;
	//final.b = vIn.b;
	//final.a = vIn.a;
//final = vIn.color;
	return vIn.color;
}