//--------------------------------------------------------------------------------------
// to mimic dispalcement mapping, we use parallax occlusion mapping
// -------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------
Texture2D txColormap : register(t2);
Texture2D txNormalmap : register(t3);
Texture2D txHeightmap : register(t4);
SamplerState txSampler : register(s2);

//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
cbuffer DirectionalLightBuffer : register(b5)
{
    float4 dirColor;
    float3 dirDirection;
    float dirIntensity;
    float3 dirPosition;
    float dirRange;
    float3 dirAttenuation;
    float dirPadding;
};

cbuffer AmbientLightBuffer : register(b6)
{
    float4 amColor;
    float3 amDirection;
    float amIntensity;
    float3 amPosition;
    float amRange;
    float3 amAttenuation;
    float amPadding;
};

cbuffer ConstantBufferCamera : register(b0)
{
    matrix View;
    matrix Projection;
    float4 eyePos;
}

cbuffer ConstantBuffer : register(b3)
{
    matrix World;
    float rotationAngle;
    bool enableRotation;
    bool enableTranslation;
}

//--------------------------------------------------------------------------------------
// Input and Output Structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD;
    float3 Norm : NORMAL;
    float3 Tang : TANGENT;
    float3 Binorm : BINORMAL;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
    float3 WorldPos : POSITION;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    float3 Normal : NORMAL;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // Transform position to world space
    float4 worldPosition = mul(float4(input.Pos, 1.0f), World);
    output.WorldPos = worldPosition.xyz;

    // Transform position to clip space
    output.Pos = mul(worldPosition, View);
    output.Pos = mul(output.Pos, Projection);

    // Pass texture coordinates
    output.Tex = input.Tex;

    // Transform tangent, binormal, and normal vectors to world space
    output.Tangent = normalize(mul(float4(input.Tang, 0.0f), World).xyz);
    output.Binormal = normalize(mul(float4(input.Binorm, 0.0f), World).xyz);
    output.Normal = normalize(mul(float4(input.Norm, 0.0f), World).xyz);

    return output;
}

//--------------------------------------------------------------------------------------
// Parallax Occlusion Mapping Function
//--------------------------------------------------------------------------------------
float2 ParallaxOffset(float3 viewDir, float2 texCoords)
{
    // Simulate depth using height map
    float height = txHeightmap.Sample(txSampler, texCoords).r;
    float scale = 0.1f; // Adjust depth scale
    float bias = -0.05f;

    // Calculate parallax offset
    float parallax = height * scale + bias;
    return texCoords + viewDir.xy * parallax;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    // Construct TBN matrix
    float3x3 TBN = float3x3(input.Tangent, input.Binormal, input.Normal);

    // Compute view direction in tangent space
    float3 viewDir = normalize(mul(eyePos.xyz - input.WorldPos, TBN));

    // Apply parallax occlusion mapping
    float2 parallaxTexCoords = ParallaxOffset(viewDir, input.Tex);

    // Sample normal from normal map using parallax-corrected texture coordinates
    float3 normalFromMap = txNormalmap.Sample(txSampler, parallaxTexCoords).rgb;
    normalFromMap = normalize(normalFromMap * 2.0f - 1.0f); // Map normal to [-1, 1]

    // Transform normal from tangent space to world space
    float3 worldNormal = normalize(mul(normalFromMap, TBN));

    // Compute ambient lighting
    float3 ambient = amColor.rgb * amIntensity;

    // Compute directional lighting
    float3 lightDir = normalize(-dirDirection);
    float NdotL = max(dot(worldNormal, lightDir), 0.0f);
    float3 diffuse = NdotL * dirColor.rgb * dirIntensity;

    // Compute view direction
    float3 reflectDir = reflect(-lightDir, worldNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16); // Shininess = 16
    float3 specular = spec * dirColor.rgb * dirIntensity;

    // Sample base color using parallax-corrected texture coordinates
    float4 baseColor = txColormap.Sample(txSampler, parallaxTexCoords);

    // Combine lighting
    float3 lighting = ambient + diffuse + specular;

    // Return final color
    return float4(lighting * baseColor.rgb, baseColor.a);
}
