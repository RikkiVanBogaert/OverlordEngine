//=============================================================================
// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

// Create Depth Stencil State (ENABLE DEPTH WRITING)
// Create Rasterizer State (Backface culling) 

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
    CullMode = BACK;
};

// IN/OUT STRUCTS
// --------------
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
    output.Position = float4(input.Position, 1.0f);
    output.TexCoord = input.TexCoord;
    return output;
}

// PIXEL SHADER
// ------------
float4 PS(PS_INPUT input) : SV_Target
{
    // Step 1: Sample the texture
    float4 center = gTexture.Sample(samPoint, input.TexCoord);

    // Step 2: Sample neighboring pixels
    float textureWidth = 0.f;
    float textureHeight = 0.f;
    gTexture.GetDimensions(textureWidth, textureHeight);

    float2 texelSize = float2(1.0f / textureWidth, 1.0f / textureHeight);
    float4 top = gTexture.Sample(samPoint, input.TexCoord + float2(0.0f, texelSize.y));
    float4 bottom = gTexture.Sample(samPoint, input.TexCoord - float2(0.0f, texelSize.y));
    float4 left = gTexture.Sample(samPoint, input.TexCoord - float2(texelSize.x, 0.0f));
    float4 right = gTexture.Sample(samPoint, input.TexCoord + float2(texelSize.x, 0.0f));

    // Step 3: Compute the average color
    float4 averageColor = (center + top + bottom + left + right) / 5.0f;

    // Step 4: Adjust the blending factor
    float blendingFactor = .5f; // Adjust this value to control the blending strength

    // Step 5: Blend the average color with the original texture color
    float4 finalColor = lerp(center, averageColor, blendingFactor);

    return finalColor;
}


//TECHNIQUE
//---------
technique11 Grayscale
{
    pass P0
    {          
        // Set states...
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

