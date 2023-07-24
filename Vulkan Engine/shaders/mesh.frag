#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPositionWorld;
layout(location = 2) in vec3 fragNormalWorld; 
layout(location = 3) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    vec4 ambientLightColor; // w is intensity
    PointLight lightPoints[10];
    int numLights;
} ubo;

layout(set = 1, binding = 0) uniform sampler2D image;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix; 
} push;

void main() {
    vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 specularLight = vec3(0.f);
    vec3 surfaceNormal = normalize(fragNormalWorld);

    vec3 cameraPositionWorld = ubo.inverseView[3].xyz;
    vec3 viewDirection = normalize(cameraPositionWorld - fragPositionWorld);

    for (int i = 0; i < ubo.numLights; i++) {
        PointLight light = ubo.lightPoints[i];
        vec3 directionToLight = light.position.xyz - fragPositionWorld;
        float attenuation = 1.0 / dot(directionToLight, directionToLight);
        directionToLight = normalize(directionToLight);

        float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
        vec3 intensity = light.color.xyz * light.color.w * attenuation;

        diffuseLight += intensity * cosAngIncidence;

        vec3 halfAngle = normalize(directionToLight + viewDirection);
        // x = pow(x1, 32.0) -> x1 = clamp(x2, 0, 1) -> x2 = dot(surf, ha)
        float blinnTerm = pow(clamp(dot(surfaceNormal, halfAngle), 0, 1), 512.0);
        specularLight += intensity * blinnTerm;
    }

    vec3 imageColor = texture(image, fragUV).rgb;

    outColor = vec4((diffuseLight * imageColor + specularLight * imageColor), 1.0);
}