#version 330

layout (location = 0) out vec4 outColour;
layout (location = 1) out vec4 outBrightColour;

uniform vec3 lightPosition;

in vec3 passFragPosition;
in vec2 passTextureCoord;
in vec3 passNormal;

uniform sampler2D tex;
uniform sampler2D norm;

void main() {

    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(norm, passTextureCoord).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0) + normalize(passNormal); 


    outColour = vec4(1.0, 0.5, 0.5, 1.0);
    //vec3 normal = normalize(passNormal);
    vec3 lightDirection = normalize(lightPosition - passFragPosition);

    float diff = max(dot(normal, lightDirection) * 1.3, 0.3);
    vec3 diffuse = texture(tex, passTextureCoord).rgb * diff;

    outColour = vec4(diffuse, 1.0f);

    float brightness = dot(outColour.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.9) 
        outBrightColour = vec4(outColour.rgb, 1.0);
    else
        outBrightColour = vec4(0.0, 0.0, 0.0, 1.0); 
}