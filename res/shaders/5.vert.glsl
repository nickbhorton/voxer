#version 450 core

in vec4 v_position;
in uint v_data;

out vec4 f_color;
out float f_dim;

uniform mat4 view;
uniform mat4 proj;

uniform vec3 offset;

uniform vec4 colors[256];

void main() {
    vec4 offset_pos = vec4(
        float((v_data >> 0 ) & 255),
        float((v_data >> 8 ) & 255),
        float((v_data >> 16) & 255),
        0.0
    );
    vec4 base_pos = v_position;

    /*
    0: down
    1: up
    2: north
    3: east
    4: south
    5: west
    */
    f_dim = 1.0;
    uint direction = gl_InstanceID % 6;
    if (direction == 1) {
        base_pos.y = 1.0 - base_pos.y;
        base_pos.x = 1.0 - base_pos.x;
    }
    else if (direction == 2) {
        base_pos.y = base_pos.y + 1.0;
        base_pos.yz = base_pos.zy;
        f_dim = 0.86;
    }
    else if (direction == 3) {
        base_pos.x = 1.0 - base_pos.x;
        base_pos.xy = base_pos.yx;
        f_dim = 0.8;
    }
    else if (direction == 4) {
        base_pos.z = 1.0 - base_pos.z;
        base_pos.yz = base_pos.zy;
        f_dim = 0.86;
    }
    else if (direction == 5) {
        base_pos.y = base_pos.y + 1.0;
        base_pos.xy = base_pos.yx;
        f_dim = 0.8;
    }

    // color
    uint ci = (v_data >> 24) & 255;
    f_color = colors[ci];

    gl_Position = proj * view * (vec4(offset, 0.0) + base_pos + offset_pos);
}
