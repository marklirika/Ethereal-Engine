#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
//layout(location = 4 ) in vec3 tangent;

layout (set = 0, binding = 0) uniform UBO {
	mat4 projection;
	mat4 model;
	mat4 view;
} ubo;

layout(location = 0) out vec3 fragPositionWorld;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec2 fragUv;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {
	vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);

	fragPositionWorld = positionWorld.xyz;
	fragColor = color;
	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
	fragUv = uv;

	gl_Position = ubo.projection * ubo.view * positionWorld;
	//no tangent for now
	//outTangent = mNormal * normalize(inTangent);
}	
