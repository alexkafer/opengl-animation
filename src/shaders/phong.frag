#version 150 core

out vec4 outColor;

in vec3 vposition;
in vec3 vnormal;
in vec2 vtexture_coord;
in vec3 vcolor;

uniform vec3 eye;
uniform vec4 lightPosition;             // should be in the eye space
uniform vec4 lightAmbient;              // light ambient color
uniform vec4 lightDiffuse;              // light diffuse color
uniform vec4 lightSpecular;             // light specular color
uniform vec4 materialAmbient;           // material ambient color
uniform vec4 materialDiffuse;           // material diffuse color
uniform vec4 materialSpecular;          // material specular color
uniform float materialShininess;        // material specular shininess

// uniform sampler2D texture_map;
uniform sampler2D texture_map;                 // texture map #1
uniform bool texture_used;


vec3 phong(vec3 normal, vec3 light, vec3 view) {
    vec3 color = vcolor * materialAmbient.rgb;        // begin with ambient
    
    float dotNL = max(dot(normal, light), 0.0);
    color += vcolor * materialDiffuse.rgb * dotNL;    // add diffuse

    if(texture_used) {
        color *= texture(texture_map, vtexture_coord).rgb;                // modulate texture map
    }

    if( false && dot(normal, light) > 0.0 ){
        vec3 reflectDir = reflect(-light, normal);  
        float dotNH = max(dot(view, reflectDir), 0.0);
        color += pow(dotNH, materialShininess) * lightSpecular.rgb * materialSpecular.rgb; // add specular
    }

    return color;
}

vec3 get_light() {
    if(lightPosition.w == 0.0)
    {
        return normalize(lightPosition.xyz);
    }
    else
    {
        return normalize(lightPosition.xyz - vposition);
    }
}

void main(){
	vec3 normal = normalize(vnormal);

    
    vec3 light_dir = get_light();

    vec3 view = normalize(eye-vposition);

    if (dot(normal, light_dir) < 0) {
        normal *= -1;
    }

	vec3 result = phong( normal, light_dir, view );
	outColor = vec4(result, materialDiffuse.a);
    // outColor = vec4(0,0,1,1);
} 