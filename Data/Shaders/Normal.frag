#version 330

layout (location = 0) out vec4 outColour;
layout (location = 1) out vec4 outBrightColour;

uniform vec3 lightPosition;

in vec3 passFragPosition;
in vec2 passTextureCoord;
in vec3 passNormal;

uniform sampler2D colourTexture;
uniform sampler2D normalTexture;

void main() 
{
    vec3 colourTex = texture(colourTexture, passTextureCoord).rgb;
    vec3 normalTex = texture(normalTexture, passTextureCoord).rgb;
    normalTex = normalize(normalTex * 2.0 - 1.0) + normalize(passNormal); 

    vec3    lightDirection  = normalize(lightPosition - passFragPosition);
    float   diff            = max(dot(normalTex, lightDirection), 0.25);
    vec3    diffuse         = colourTex * diff;
    outColour = vec4(diffuse, 1.0f);

    float brightness = dot(outColour.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.9) 
        outBrightColour = vec4(outColour.rgb, 1.0);
    else
        outBrightColour = vec4(0.0, 0.0, 0.0, 1.0); 
}