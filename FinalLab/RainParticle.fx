//--------------------------------------------------------------------------------------
// not being used as rain is not properly rendered
//--------------------------------------------------------------------------------------

cbuffer ConstantBufferCamera : register(b0)
{
    matrix View;
    matrix Projection;
    float4 eyePos; // eyePos.w is the time
};

struct ParticleInput
{
    float3 position : POSITION;
    float3 velocity : VELOCITY;
};

struct ParticleOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// Vertex Shader
ParticleOutput VS(ParticleInput input)
{
    ParticleOutput output;

    // Calculate new position based on velocity and time
    float time = eyePos.w; // Time passed from the camera buffer
    float3 newPosition = input.position + input.velocity * time;

    // Transform position to clip space
    float4 worldPosition = float4(newPosition, 1.0);
    float4 viewPosition = mul(View, worldPosition);
    output.position = mul(Projection, viewPosition);

    // Assign color based on height (Y-axis)
    float heightFactor = saturate((newPosition.y - 0.0) / 10.0); // Assuming 10.0 is max height
   // output.color = lerp(float4(0.5, 0.5, 1.0, 1.0), float4(0.0, 0.0, 1.0, 1.0), heightFactor);
    output.color = float4(1.0, 0.0, 0.0, 1.0);
    return output;
}

// Pixel Shader
float4 PS(ParticleOutput input) : SV_TARGET
{
    // Pass the color directly to the output
    return input.color;
}
