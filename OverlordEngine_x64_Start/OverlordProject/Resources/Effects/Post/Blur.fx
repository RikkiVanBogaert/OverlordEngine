//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

 
DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = BACK;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


// VERTEX SHADER
// -------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    // Set the Position
    output.Position = float4(input.Position, 1.0f);

    // Set the TexCoord
    output.TexCoord = input.TexCoord;

    return output;
}


// PIXEL SHADER
// ------------
float4 PS(PS_INPUT input) : SV_Target
{
	// Step 1: Find the dimensions of the texture (the texture has a method for that)
	float textureWidth = 0.f;
	float textureHeight = 0.f;
	gTexture.GetDimensions(textureWidth, textureHeight);

	// Step 2: Calculate dx and dy (UV space for 1 pixel)
	float2 dx = float2(1.0f / textureWidth, 0.0f);
	float2 dy = float2(0.0f, 1.0f / textureHeight);

	// Step 3: Create a double for loop (5 iterations each)
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int iterations = 5;

	float startW = input.TexCoord.x - dx.x * (iterations / 2);
	float startH = input.TexCoord.y - dy.y * (iterations / 2);

	for (int w = 0; w < iterations; ++w)
	{
		for (int h = 0; h < iterations; ++h)
		{
			float2 samUV = float2(startW + (w * dx.x), startH + (h * dy.y));
			finalColor = finalColor + gTexture.Sample(samPoint, samUV);
		}
	}

	// Step 4: Divide the final color by the number of passes (in this case 5*5)
	finalColor /= (iterations * iterations);

	// Step 5: Return the final color
	return finalColor;
}



//TECHNIQUE
//---------
technique11 Blur
{
    pass P0
    {
		// Set states...
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}