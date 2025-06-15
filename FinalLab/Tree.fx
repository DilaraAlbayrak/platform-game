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

cbuffer ConstantBuffer : register(b4)
{
    matrix World;
    float rotationAngle;
    bool enableRotation;
    bool enableTranslation;
}

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 WorldPos : POSITION;
    float4 Color : COLOR;
};

VS_OUTPUT VS(float3 Pos : POSITION, float3 Normal : NORMAL, float4 VertexColor : COLOR)
{
    VS_OUTPUT output;

    float4 worldPosition = mul(float4(Pos, 1.0f), World);
    output.WorldPos = worldPosition.xyz;
    output.Pos = mul(worldPosition, mul(View, Projection));

    float3 normal = normalize(mul(float4(Normal, 0.0f), World).xyz);
    float3 viewDir = normalize(eyePos.xyz - output.WorldPos);

    float3 ambient = amColor.rgb * amIntensity;

    float3 lightDir = normalize(-dirDirection);
    float NdotL = max(dot(normal, lightDir), 0.0f);
    float3 diffuse = NdotL * dirColor.rgb * dirIntensity;

    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    float3 specular = spec * dirColor.rgb * dirIntensity;

    float3 lighting = ambient + diffuse + specular;

    output.Color.rgb = lighting * VertexColor.rgb;
    output.Color.a = VertexColor.a;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    return input.Color;
}