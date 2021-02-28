#version 330

layout (location = 0) out vec4 outColour;
layout (location = 1) out vec4 outBrightColour;

uniform vec3 lightPosition;

in vec3 passFragPosition;
flat in vec3 passNormal;
in vec4 passColour;

uniform sampler2D textures;

void main() {
    vec3 normal = normalize(passNormal);

    vec3    lightDirection  = normalize(lightPosition - passFragPosition);
    float   diff            = max(dot(normal, lightDirection), 0.25);


    vec3    diffuse         = passColour.rgb * diff;
    outColour = vec4(diffuse, passColour.a);



    float brightness = dot(outColour.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.9) 
        outBrightColour = vec4(outColour.rgb, 1.0);
    else
        outBrightColour = vec4(0.0, 0.0, 0.0, 1.0); 
}