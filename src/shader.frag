#version 150 core

out vec4 outColor;

in vec3 vposition;
in vec3 vcolor;
in vec3 vnormal;

uniform vec3 eye;
uniform vec4 lightPosition;             // should be in the eye space
uniform vec4 lightAmbient;              // light ambient color
uniform vec4 lightDiffuse;              // light diffuse color
// uniform vec4 lightSpecular;             // light specular color
uniform vec4 materialAmbient;           // material ambient color
uniform vec4 materialDiffuse;           // material diffuse color
// uniform vec4 materialSpecular;          // material specular color
// uniform float materialShininess;        // material specular shininess

//
//	Diffuse color
//
vec4 diffuse(vec3 light_dir, vec3 normal){
	// Color coeffs
	vec4 ambient = 0.1f * lightAmbient * materialAmbient;
	float diff = max( dot(normal, -1.f * light_dir), 0.f);
	vec4 diffuse = diff * lightDiffuse * materialDiffuse;

	return ( diffuse );
}

vec3 get_light() {
    if(lightPosition.w == 0.0)
    {
        return normalize(lightPosition.xyz);
    }
    else
    {
        return normalize(vposition - lightPosition.xyz);
    }
}

void main() {
    vec3 normal = normalize(vnormal);
	vec3 view = normalize(vposition-eye);

    // if( dot(normal, view) < 0.0 ){ normal *= -1.0; } // draw two-sided
    vec3 light_dir = get_light();
    vec4 result = diffuse(light_dir, normal);

    outColor = vec4(result.rgb, materialDiffuse.a); //( Red, Green, Blue, Alpha)
}