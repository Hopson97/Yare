#version 450

layout (location = 0) in vec3 inVertexCoord;
layout (location = 1) in vec4 inColour;
layout (location = 2) in vec3 inNormal;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;
uniform vec3 lightPosition;

flat out vec4 passColour;

out vec4 

uniform vec4 clippingPlane;

void calcuateVertexColour(vec3 fragmentPos)
{
    vec3  normal          = normalize(mat3(modelMatrix) * inNormal);
    vec3  lightDirection  = normalize(lightPosition - fragmentPos);
    float diff            = max(dot(normal, lightDirection), 0.25);
    vec3  diffuse         = inColour.rgb * diff;
    passColour = vec4(diffuse, inColour.a);
}

void main() 
{
    vec4 worldPos = modelMatrix * vec4(inVertexCoord, 1.0);    
    gl_ClipDistance[0] = dot(worldPos, clippingPlane);
    gl_Position = projectionViewMatrix * worldPos;

    calcuateVertexColour(vec3(modelMatrix * vec4(inVertexCoord, 1.0)));
}