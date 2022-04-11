#version 460 core

layout(r8ui, binding = 0) uniform uimage2D values;

uniform int iteration;

in vec2 texCoords;

out vec4 fragColor;

void main() {
    ivec2 size = imageSize(values);
    vec2 uv = (2.0 * gl_FragCoord.xy - size) / size;

    ivec2 idx = ivec2(texCoords * size);
    uint status = imageLoad(values, idx).x;


    fragColor = vec4(status * vec3(texCoords.x * texCoords.y, 1.0 - texCoords.xy), 1);
}
