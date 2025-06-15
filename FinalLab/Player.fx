//--------------------------------------------------------------------------------------
// Constant Buffer Variables
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

cbuffer SpotLightBuffer : register(b7)
{
    float4 spotColor;
    float3 spotDirection;
    float spotIntensity;
    float3 spotPosition;
    float spotRange;
    float3 spotAttenuation;
    float spotPadding;
};

cbuffer ConstantBufferCamera : register(b0)
{
    matrix View;
    matrix Projection;
    float4 eyePos;
}

cbuffer ConstantBuffer : register(b1)
{
    matrix World;
    float rotationAngle;
    bool enableRotation;
    bool enableTorch;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 WorldPos : POSITION; // world position for lighting
    float3 Normal : NORMAL;
    float4 Color : COLOR;
    float Torch : TORCH;
    float Intentsity : INTENSITY;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float3 Pos : POSITION, float4 Color : COLOR, float3 Normal : NORMAL)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    if (enableRotation && Pos.y > -2.0f)
    {
        float cosAngle = cos(rotationAngle);
        float sinAngle = sin(rotationAngle);
        
        float3 rotatedPos;
        rotatedPos.x = Pos.x * cosAngle - Pos.y * sinAngle;
        rotatedPos.y = Pos.x * sinAngle + Pos.y * cosAngle;
        rotatedPos.z = Pos.z;

        Pos = rotatedPos;
    }  
    
    float4 worldPosition = mul(float4(Pos, 1.0), World);
    output.WorldPos = worldPosition.xyz;
    output.Pos = mul(worldPosition, mul(View, Projection));
    output.Normal = normalize(mul(float4(Normal, 0.0), World).xyz);
    output.Color = Color;
    output.Torch = enableTorch ? 1.0f : 0.0f;;
    //output.Intentsity = enableTorch ? dirIntensity : 1.1f;
    output.Intentsity = dirIntensity;
    
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float3 viewDir = normalize(eyePos.xyz - input.WorldPos);

    // Ambient Light
    float3 ambient = amColor.rgb * amIntensity;

    // Directional Light
    float3 lightDir = normalize(-dirDirection);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float3 diffuse = NdotL * dirColor.rgb * input.Intentsity;
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8); // Shininess = 16
    float3 specular = spec * dirColor.rgb * input.Intentsity;

    // Spot Light
    float torchWeight = input.Torch;

    float3 spotLightDir = normalize(spotPosition - input.WorldPos);
    float spotTheta = dot(-spotLightDir, normalize(spotDirection));

    // Smooth transition for the spotlight cone using lerp
    float spotFactor = saturate((spotTheta - spotRange) / (1.0f - spotRange));

    // Calculate spotlight effects only within the cone
    float spotDistance = length(spotPosition - input.WorldPos);
    float attenuation = 1.0 / (spotAttenuation.x + spotAttenuation.y * spotDistance + spotAttenuation.z * (spotDistance * spotDistance));

    float NdotSpotL = max(dot(normal, spotLightDir), 0.0);
    float spotDiffuse = attenuation * spotIntensity * NdotSpotL * spotColor.rgb;

    float3 spotReflectDir = reflect(-spotLightDir, normal);
    float spotSpec = pow(max(dot(viewDir, spotReflectDir), 0.0), 8); // Shininess = 16
    float spotSpecular = attenuation * spotSpec * spotIntensity * spotColor.rgb;

    // Apply the spotlight factor and enableTorch weight
    spotDiffuse *= spotFactor * torchWeight;
    spotSpecular *= spotFactor * torchWeight;

    // Final lighting composition
    float3 lighting = ambient + diffuse + specular + spotDiffuse + spotSpecular;

    return float4(lighting, input.Color.a);
}