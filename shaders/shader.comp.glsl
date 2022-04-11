#version 460 core

layout(local_size_x = 1, local_size_y = 1) in;

layout(rgba32f, binding = 0) uniform image2D image;

void main() {
    ivec2 gidx = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(image);

    vec2 uv = vec2(gidx) / vec2(size);
    vec4 color = vec4(uv, 0.0, 1.0);

    imageStore(image, gidx, color);
}
