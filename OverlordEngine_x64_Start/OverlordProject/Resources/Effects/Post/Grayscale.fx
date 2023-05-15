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
    float4 texColor = gTexture.Sample(samPoint, input.TexCoord);

    // Step 2: Calculate the mean value
    float3 meanValue = (texColor.r + texColor.g + texColor.b) / 3.0f;

    // Step 3: Return the grayscale color
    return float4(meanValue, 1.0f);
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

