#version 150 core

out vec4 outColor;

uniform vec3 eye;
in vec3 vposition;
in vec3 vcolor;
in vec3 vnormal;

//
//	Lights
//
struct DirLight {
	vec3 direction;
	vec3 intensity;
};

//
//	phong illumination
//
vec3 phong_illumination(DirLight light, vec3 color, vec3 N, vec3 V){
	vec3 L = -1.f*normalize(light.direction);

	// Material values
	vec3 mamb = color;
	vec3 mdiff = color;
	vec3 mspec = vec3(.8,.8,.8);

	// Color coeffs
	vec3 ambient = 0.1f * mamb * light.intensity;

	float diff = max( dot(N, L), 0.f );
	vec3 diffuse = diff * mdiff * light.intensity;

	vec3 reflectDir = reflect(-L,N);
    
	float spec = max(dot(reflectDir, V), 0.0);
    if (dot(L, N) <= 0.0) spec = 0;
    vec3 specular = mspec*pow(spec,4);

	return ( ambient + diffuse + spec);
}

void main() {
    DirLight light;
    light.direction = vposition-eye; // define point light source at eye
	light.intensity = vec3(1,1,1);

    vec3 N = normalize(vnormal);
	vec3 V = normalize(eye-vposition);

    if( dot(N,V) < 0.0 ){ N *= -1.0; } // draw two-sided
    vec3 result = phong_illumination( light, vcolor, N, V);

    outColor = vec4(result, 1.0); //( Red, Green, Blue, Alpha)
}