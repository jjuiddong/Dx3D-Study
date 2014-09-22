//-----------------------------------------------------------------------------
// Copyright (c) 2007-2008 dhpoware. All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------
//
// This D3DX Effect file implements tangent space normal mapping.
//
// Three techniques are provided for each of the three lighting types:
// directional lighting, point lighting, and spotlight lighting.
//
// Tangent space normal mapping involves evaluating the lighting equations per
// pixel in the surface local coordinate space of the normal map texture.
// Rather than using the mesh's interpolated vertex normals tangent space
// normal mapping uses the normals from the normal map texture instead. Doing
// this allows additional surface detail to be applied to each face of the mesh
// without requiring extra geometry to be added to each face.
//
// The lighting vectors are transformed into the normal map texture's tangent
// space using a rotation matrix. This rotation matrix must be calculated for
// each face of the mesh. Each face has its own rotation matrix because each
// face is uniquely texture mapped to the normal map texture and the rotation
// matrix must take this into consideration. The rotation matrix is constructed
// using each face's tangent, bitangent, and normal vectors.
//
// The pixel shader used in tangent space normal mapping is identical to the
// pixel shader used in per-pixel lighting. The only difference is the normal
// used is read from the normal map texture rather that being interpolated by
// the rasterizer. Prior to being stored in the normal map texture the normals
// are in the range [-1,1]. Once stored in the normal map texture the normals
// will be compressed to the range [0,255]. When the normals are accessed from
// the pixel shader HLSL will remap then to the range [0,1]. To convert the
// normals back to the original [-1,1] range we need to perform a scale and a
// bias. We do this by: tex2D(normalMap, IN.texCoord) * 2.0f - 1.0f.
//
// Light attenuation for the point and spot lighting models is based on a
// light radius. Light is at its brightest at the center of the sphere defined
// by the light radius. There is no lighting at the edges of this sphere.
//
//-----------------------------------------------------------------------------

struct Light
{
	float3 dir;				// world space direction
	float3 pos;				// world space position
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float spotInnerCone;	// spot light inner cone (theta) angle
	float spotOuterCone;	// spot light outer cone (phi) angle
	float radius;           // applies to point and spot lights only
};

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 emissive;
	float4 specular;
	float shininess;
};

//-----------------------------------------------------------------------------
// Globals.
//-----------------------------------------------------------------------------

float4x4 worldMatrix;
float4x4 worldInverseTransposeMatrix;
float4x4 worldViewProjectionMatrix;

float3 cameraPos;
float4 globalAmbient;

Light light;
Material material;

//-----------------------------------------------------------------------------
// Textures.
//-----------------------------------------------------------------------------

texture colorMapTexture;
texture normalMapTexture;

sampler2D colorMap = sampler_state
{
	Texture = <colorMapTexture>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};

sampler2D normalMap = sampler_state
{
    Texture = <normalMapTexture>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};

//-----------------------------------------------------------------------------
// Vertex Shaders.
//-----------------------------------------------------------------------------

struct VS_INPUT
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
    float4 tangent : TANGENT;
};

struct VS_OUTPUT_DIR
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 halfVector : TEXCOORD1;
	float3 lightDir : TEXCOORD2;
	float4 diffuse : COLOR0;
	float4 specular : COLOR1;
};

struct VS_OUTPUT_POINT
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 viewDir : TEXCOORD1;
	float3 lightDir : TEXCOORD2;
	float4 diffuse : COLOR0;
	float4 specular : COLOR1;
};

struct VS_OUTPUT_SPOT
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 viewDir : TEXCOORD1;
	float3 lightDir : TEXCOORD2;
	float3 spotDir : TEXCOORD3;
	float4 diffuse : COLOR0;
	float4 specular : COLOR1;
};

VS_OUTPUT_DIR VS_DirLighting(VS_INPUT IN)
{
	VS_OUTPUT_DIR OUT;

	float3 worldPos = mul(float4(IN.position, 1.0f), worldMatrix).xyz;
	float3 lightDir = -light.dir;
	float3 viewDir = cameraPos - worldPos;
	float3 halfVector = normalize(normalize(lightDir) + normalize(viewDir));
		
	float3 n = mul(IN.normal, (float3x3)worldInverseTransposeMatrix);
	float3 t = mul(IN.tangent.xyz, (float3x3)worldInverseTransposeMatrix);
	float3 b = cross(n, t) * IN.tangent.w;
	float3x3 tbnMatrix = float3x3(t.x, b.x, n.x,
	                              t.y, b.y, n.y,
	                              t.z, b.z, n.z);

	OUT.position = mul(float4(IN.position, 1.0f), worldViewProjectionMatrix);
	OUT.texCoord = IN.texCoord;
	OUT.halfVector = mul(halfVector, tbnMatrix);
	OUT.lightDir = mul(lightDir, tbnMatrix);
	OUT.diffuse = material.diffuse * light.diffuse;
	OUT.specular = material.specular * light.specular;

	return OUT;
}

VS_OUTPUT_POINT VS_PointLighting(VS_INPUT IN)
{
	VS_OUTPUT_POINT OUT;

	float3 worldPos = mul(float4(IN.position, 1.0f), worldMatrix).xyz;
	float3 viewDir = cameraPos - worldPos;
	float3 lightDir = (light.pos - worldPos) / light.radius;
       
    float3 n = mul(IN.normal, (float3x3)worldInverseTransposeMatrix);
	float3 t = mul(IN.tangent.xyz, (float3x3)worldInverseTransposeMatrix);
	float3 b = cross(n, t) * IN.tangent.w;
	float3x3 tbnMatrix = float3x3(t.x, b.x, n.x,
	                              t.y, b.y, n.y,
	                              t.z, b.z, n.z);
			
	OUT.position = mul(float4(IN.position, 1.0f), worldViewProjectionMatrix);
	OUT.texCoord = IN.texCoord;
	OUT.viewDir = mul(viewDir, tbnMatrix);
	OUT.lightDir = mul(lightDir, tbnMatrix);
	OUT.diffuse = material.diffuse * light.diffuse;
	OUT.specular = material.specular * light.specular;
	
	return OUT;
}

VS_OUTPUT_SPOT VS_SpotLighting(VS_INPUT IN)
{
    VS_OUTPUT_SPOT OUT;
    
    float3 worldPos = mul(float4(IN.position, 1.0f), worldMatrix).xyz;
    float3 viewDir = cameraPos - worldPos;
	float3 lightDir = (light.pos - worldPos) / light.radius;
    
    float3 n = mul(IN.normal, (float3x3)worldInverseTransposeMatrix);
	float3 t = mul(IN.tangent.xyz, (float3x3)worldInverseTransposeMatrix);
	float3 b = cross(n, t) * IN.tangent.w;
	float3x3 tbnMatrix = float3x3(t.x, b.x, n.x,
	                              t.y, b.y, n.y,
	                              t.z, b.z, n.z);
		       
    OUT.position = mul(float4(IN.position, 1.0f), worldViewProjectionMatrix);
	OUT.texCoord = IN.texCoord;
	OUT.viewDir = mul(viewDir, tbnMatrix);
	OUT.lightDir = mul(lightDir, tbnMatrix);
    OUT.spotDir = mul(light.dir, tbnMatrix);
    OUT.diffuse = material.diffuse * light.diffuse;
	OUT.specular = material.specular * light.specular;
       
    return OUT;
}

//-----------------------------------------------------------------------------
// Pixel Shaders.
//-----------------------------------------------------------------------------

float4 PS_DirLighting(VS_OUTPUT_DIR IN) : COLOR
{
    float3 n = normalize(tex2D(normalMap, IN.texCoord).rgb * 2.0f - 1.0f);
    float3 h = normalize(IN.halfVector);
    float3 l = normalize(IN.lightDir);
    
    float nDotL = saturate(dot(n, l));
    float nDotH = saturate(dot(n, h));
    float power = (nDotL == 0.0f) ? 0.0f : pow(nDotH, material.shininess);
    
    float4 color = (material.ambient * (globalAmbient + light.ambient)) +
                   (IN.diffuse * nDotL) + (IN.specular * power);

	return color * tex2D(colorMap, IN.texCoord);
}

float4 PS_PointLighting(VS_OUTPUT_POINT IN) : COLOR
{
    float atten = saturate(1.0f - dot(IN.lightDir, IN.lightDir));

	float3 n = normalize(tex2D(normalMap, IN.texCoord).rgb * 2.0f - 1.0f);
    float3 l = normalize(IN.lightDir);
    float3 v = normalize(IN.viewDir);
    float3 h = normalize(l + v);
    
    float nDotL = saturate(dot(n, l));
    float nDotH = saturate(dot(n, h));
    float power = (nDotL == 0.0f) ? 0.0f : pow(nDotH, material.shininess);

    float4 color = (material.ambient * (globalAmbient + (atten * light.ambient))) +
                   (IN.diffuse * nDotL * atten) + (IN.specular * power * atten);
                   
	return color * tex2D(colorMap, IN.texCoord);
}

float4 PS_SpotLighting(VS_OUTPUT_SPOT IN) : COLOR
{
    float atten = saturate(1.0f - dot(IN.lightDir, IN.lightDir));
    
	float3 l = normalize(IN.lightDir);
    float2 cosAngles = cos(float2(light.spotOuterCone, light.spotInnerCone) * 0.5f);
    float spotDot = dot(-l, normalize(IN.spotDir));
    float spotEffect = smoothstep(cosAngles[0], cosAngles[1], spotDot);
    
    atten *= spotEffect;

    float3 n = normalize(tex2D(normalMap, IN.texCoord).rgb * 2.0f - 1.0f);
	float3 v = normalize(IN.viewDir);
	float3 h = normalize(l + v);
    
    float nDotL = saturate(dot(n, l));
    float nDotH = saturate(dot(n, h));
    float power = (nDotL == 0.0f) ? 0.0f : pow(nDotH, material.shininess);
    
    float4 color = (material.ambient * (globalAmbient + (atten * light.ambient))) +
                   (IN.diffuse * nDotL * atten) + (IN.specular * power * atten);
    
	return color * tex2D(colorMap, IN.texCoord);
}

//-----------------------------------------------------------------------------
// Techniques.
//-----------------------------------------------------------------------------

technique NormalMappingDirectionalLighting
{
	pass
	{
		VertexShader = compile vs_2_0 VS_DirLighting();
		PixelShader = compile ps_2_0 PS_DirLighting();
	}
}

technique NormalMappingPointLighting
{
    pass
    {
        VertexShader = compile vs_2_0 VS_PointLighting();
        PixelShader = compile ps_2_0 PS_PointLighting();
    }
}

technique NormalMappingSpotLighting
{
    pass
    {
        VertexShader = compile vs_2_0 VS_SpotLighting();
        PixelShader = compile ps_2_0 PS_SpotLighting();
    }
}
