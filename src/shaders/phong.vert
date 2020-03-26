#version 150 core

in vec3 in_position;
in vec3 in_normal;
in vec4 in_color;
in vec2 in_texture_coord;

in ivec4 in_bone_ids;
in vec4 in_bone_weights;

out vec3 vposition;
out vec4 vcolor;
out vec3 vnormal;
out vec2 vtexture_coord;

const int MAX_BONES = 100;
uniform bool has_bones;

uniform mat4 camera;
uniform mat4 model;
uniform mat4 bones[MAX_BONES];

void main() {
    vtexture_coord = in_texture_coord;
    vcolor = in_color;

    vec4 PosL;
    vec4 NormalL;

    if (has_bones) {
        mat4 BoneTransform = bones[in_bone_ids[0]] * in_bone_weights[0];
        BoneTransform += bones[in_bone_ids[1]] * in_bone_weights[1];
        BoneTransform += bones[in_bone_ids[2]] * in_bone_weights[2];
        BoneTransform += bones[in_bone_ids[3]] * in_bone_weights[3];

        PosL = BoneTransform *  vec4(in_position, 1.0);
        NormalL = BoneTransform * vec4(in_normal, 0.0);
    } else {
        PosL = vec4(in_position, 1.0);
        NormalL = vec4(in_normal, 0.0);
    }
    
    // Apply all matrix transformations to vert
    vposition = (model * PosL).xyz;
    vnormal = (model * NormalL).xyz;
    // vnormal = NormalL.xyz;

    gl_Position = camera * model * PosL;
}

