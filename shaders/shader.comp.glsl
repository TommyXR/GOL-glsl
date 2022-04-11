#version 460 core

layout(local_size_x = 1, local_size_y = 1) in;

layout(r8ui, binding = 0) uniform uimage2D values_in;
layout(r8ui, binding = 1) uniform uimage2D values_out;

uint updateCell(ivec2 ix) {

    uint status = imageLoad(values_in, ix).x;

    uint alive = 0;
    alive += imageLoad(values_in, ix + ivec2(-1, -1)).x;
    alive += imageLoad(values_in, ix + ivec2(-1, 0)).x;
    alive += imageLoad(values_in, ix + ivec2(-1, 1)).x;
    alive += imageLoad(values_in, ix + ivec2(0, -1)).x;
    alive += imageLoad(values_in, ix + ivec2(0, 1)).x;
    alive += imageLoad(values_in, ix + ivec2(1, -1)).x;
    alive += imageLoad(values_in, ix + ivec2(1, 0)).x;
    alive += imageLoad(values_in, ix + ivec2(1, 1)).x;


    return uint(status == 0 && alive == 3) + uint(status == 1 && alive > 1 && alive < 4);
}


void main() {
    ivec2 gidx = ivec2(gl_GlobalInvocationID.xy);

    uint status = updateCell(gidx);

    imageStore(values_out, gidx, uvec4(status));
}
