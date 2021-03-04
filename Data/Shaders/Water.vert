#version 450

layout (location = 0) in vec3 inVertexCoord;
layout (location = 1) in vec4 inColour;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTextureCoord;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;
uniform vec3 lightPosition;

flat out vec4 passColour;
out vec2 passTextureCoord;

uniform vec4 clippingPlane;

void calcuateVertexColour(vec3 fragmentPos)
{
    vec3  normal          = normalize(mat3(modelMatrix) * inNormal);
    vec3  lightDirection  = normalize(lightPosition - fragmentPos);
    float diff            = max(dot(normal, lightDirection), 0.25);
    vec3  diffuse         = inColour.rgb * diff;
    passColour = vec4(diffuse, inColour.a);
}

vec4 getWorldPos()
{
    vec4 worldPos = modelMatrix * vec4(inVertexCoord, 1.0);
    //worldPos.y += sin((time + worldPos.x) * 1.5) / 10.0;
    //worldPos.y += cos((time + worldPos.z) * 1.5) / 10.0;
    return worldPos;
}

void main() 
{
    vec4 worldPos = getWorldPos(); 
    //gl_ClipDistance[0] = dot(worldPos, clippingPlane);
    gl_Position = projectionViewMatrix * worldPos;

    calcuateVertexColour(vec3(modelMatrix * vec4(inVertexCoord, 1.0)));
    passTextureCoord = inTextureCoord;
}