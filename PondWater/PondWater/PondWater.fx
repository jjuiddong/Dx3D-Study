//=============================================================================
// Water.fx by Frank Luna (C) 2004 All Rights Reserved.
//
// Scrolls normal maps over for per pixel lighting of high 
// frequency waves; uses an environment map for reflections.
//=============================================================================

struct Mtrl
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float  specPower;
};

struct DirLight
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 dirW;  
};

uniform extern float4x4 gWorld;
uniform extern float4x4 gWorldInv;
uniform extern float4x4 gWVP;
uniform extern Mtrl     gMtrl;
uniform extern DirLight gLight;
uniform extern float3   gEyePosW;
uniform extern float2   gWaveMapOffset0;
uniform extern float2   gWaveMapOffset1;
uniform extern texture  gWaveMap0;
uniform extern texture  gWaveMap1;
uniform extern float    gRefractBias;
uniform extern float    gRefractPower;
uniform extern float2   gRippleScale;
uniform extern texture  gReflectMap;
uniform extern texture  gRefractMap;


sampler ReflectMapS = sampler_state
{
	Texture = <gReflectMap>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP;
    AddressV  = CLAMP;
};

sampler RefractMapS = sampler_state
{
	Texture = <gRefractMap>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP;
    AddressV  = CLAMP;
};

sampler WaveMapS0 = sampler_state
{
	Texture = <gWaveMap0>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler WaveMapS1 = sampler_state
{
	Texture = <gWaveMap1>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};


struct OutputVS
{
    float4 posH        : POSITION0;
    float3 toEyeT      : TEXCOORD0;
    float3 lightDirT   : TEXCOORD1;
    float2 tex0        : TEXCOORD2;
    float2 tex1        : TEXCOORD3;
    float4 projTexC    : TEXCOORD4;
    float  eyeVertDist : TEXCOORD5;
};


OutputVS WaterVS(float3 posL : POSITION0, float2 tex0 : TEXCOORD0)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;
	
	// Build TBN-basis.  For flat water grid in the xz-plane in 
	// object space, the TBN-basis has a very simple form.
	float3x3 TBN;
	TBN[0] = float3(1.0f, 0.0f, 0.0f); // Tangent goes along object space x-axis.
	TBN[1] = float3(0.0f, 0.0f, -1.0f);// Binormal goes along object space -z-axis
	TBN[2] = float3(0.0f, 1.0f, 0.0f); // Normal goes along object space y-axis
	
	// Matrix transforms from object space to tangent space.
	float3x3 toTangentSpace = transpose(TBN);
	
	// Transform eye position to local space.
	float3 eyePosL = mul(float4(gEyePosW, 1.0f), gWorldInv).xyz;
	outVS.eyeVertDist = distance(eyePosL, posL);
	
	// Transform to-eye vector to tangent space.
	float3 toEyeL = eyePosL - posL;
	outVS.toEyeT = mul(toEyeL, toTangentSpace);
	
	// Transform light direction to tangent space.
	float3 lightDirL = mul(float4(gLight.dirW, 0.0f), gWorldInv).xyz;
	outVS.lightDirT  = mul(lightDirL, toTangentSpace);
	
	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	
	// Scroll texture coordinates.
	outVS.tex0 = tex0 + gWaveMapOffset0;
	outVS.tex1 = tex0 + gWaveMapOffset1;
	
	// Generate projective texture coordinates from camera's perspective.
	outVS.projTexC = outVS.posH;
		
	// Done--return the output.
    return outVS;
}

float4 WaterPS(float3 toEyeT      : TEXCOORD0,
               float3 lightDirT   : TEXCOORD1,
               float2 tex0        : TEXCOORD2,
               float2 tex1        : TEXCOORD3,
               float4 projTexC    : TEXCOORD4,
               float  eyeVertDist : TEXCOORD5) : COLOR
{
	// Interpolated normals can become unnormal--so normalize.
	// Note that toEyeW and normalW do not need to be normalized
	// because they are just used for a reflection and environment
	// map look-up and only direction matters.
	toEyeT    = normalize(toEyeT);
	lightDirT = normalize(lightDirT);
	
	// Light vector is opposite the direction of the light.
	float3 lightVecT = -lightDirT;
	
	// Sample normal map.
	float3 normalT0 = tex2D(WaveMapS0, tex0);
	float3 normalT1 = tex2D(WaveMapS1, tex1);
	
	// Expand from [0, 1] compressed interval to true [-1, 1] interval.
    normalT0 = 2.0f*normalT0 - 1.0f;
    normalT1 = 2.0f*normalT1 - 1.0f;
    
	// Average the two vectors.
	float3 normalT = normalize(0.5f*(normalT0 + normalT1));
	
	// Compute the reflection vector.
	float3 r = reflect(-lightVecT, normalT);
	
	// Determine how much (if any) specular light makes it into the eye.
	float t  = pow(max(dot(r, toEyeT), 0.0f), gMtrl.specPower);
	
	// Determine the diffuse light intensity that strikes the vertex.
	float s = max(dot(lightVecT, normalT), 0.0f);
	
	// If the diffuse light intensity is low, kill the specular lighting term.
	// It doesn't look right to add specular light when the surface receives 
	// little diffuse light.
	if(s <= 0.0f)
	     t = 0.0f;
	     
	// Project the texture coordinates and scale/offset to [0,1].
	projTexC.xy /= projTexC.w;            
	projTexC.x =  0.5f*projTexC.x + 0.5f; 
	projTexC.y = -0.5f*projTexC.y + 0.5f;

	// To avoid clamping artifacts near the bottom screen edge, we 
	// scale the perturbation magnitude of the v-coordinate so that
	// when v is near the bottom edge of the texture (i.e., v near 1.0),
	// it doesn't cause much distortion.  The following power function
	// scales v very little until it gets near 1.0.
	// (Plot this function to see how it looks.)
	float vPerturbMod = -pow(abs(projTexC.y), 10.0f) + 1.0f;
	
	// Sample reflect/refract maps and perturb texture coordinates.
	float2 perturbVec = normalT.xz*gRippleScale;
	perturbVec.y *= vPerturbMod;
	float3 reflectCol = tex2D(ReflectMapS, projTexC.xy+perturbVec).rgb;
	float3 refractCol = tex2D(RefractMapS, projTexC.xy+perturbVec).rgb;
	
	// Refract based on view angle.
	float refractWt = saturate(gRefractBias+pow(max(dot(toEyeT, normalT), 0.0f), gRefractPower));
	
	// Weighted average between the reflected color and refracted color, modulated
	// with the material.
	float3 ambientMtrl = gMtrl.ambient*lerp(reflectCol, refractCol, refractWt);
	float3 diffuseMtrl = gMtrl.diffuse*lerp(reflectCol, refractCol, refractWt);
	
	// Compute the ambient, diffuse and specular terms separatly. 
	float3 spec = t*(gMtrl.spec*gLight.spec).rgb;
	float3 diffuse = (diffuseMtrl*gLight.diffuse.rgb);
	float3 ambient = ambientMtrl*gLight.ambient;
	
	float3 final = diffuse + spec;
	
	// Output the color and the alpha.
    return float4(final, gMtrl.diffuse.a);
}

technique WaterTech
{
    pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 WaterVS();
        pixelShader  = compile ps_2_0 WaterPS();
    }    
}
