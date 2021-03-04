#version 450

layout (location = 0) out vec4 outColour;
layout (location = 1) out vec4 outBrightColour;

flat in vec4 passColour;
in vec2 passTextureCoord;

uniform sampler2D reflectionSampler;

void main() 
{
    vec3 reflectTexture = texture(reflectionSampler, passTextureCoord).rgb;
    outColour = passColour;

    outColour = mix(passColour, vec4(reflectTexture, 0.6), 0.5);

    float brightness = dot(outColour.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 0.9)  {
        outBrightColour = vec4(outColour.rgb, 1.0);
    }
    else  {
        outBrightColour = vec4(0.0, 0.0, 0.0, 1.0); 
    }
}