#version 400 core
										
out vec4 FragColor;
 
in vec3 outPoz;
in vec3 outColor;
in vec3 outNormal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 cameraFront;

//uniform sampler2D ourTexture;

vec3 lighting(vec3 pos, vec3 normal, vec3 lightPos, vec3 viewPos, vec3 ambient, vec3 diffuse, vec3 specular, float specPower);
float dropOffFuction(float x);

void main()
{
	vec3 ambient = vec3(0.2);
	vec3 diffuse = vec3(1,1, 1);
	vec3 specular = vec3(0.2);
	float specPower = 4;

	vec3 color = lighting(outPoz, outNormal, lightPos, viewPos, ambient, diffuse, specular, specPower);

	FragColor = vec4(color, 1.0) * vec4(outColor, 1.0) ;
	//FragColor = vec4(outColor, 1.0);

}


vec3 lighting(vec3 pos, vec3 normal, vec3 lightPos, vec3 viewPos, vec3 ambient, vec3 diffuse, vec3 specular, float specPower)
{
	vec3 V = normalize(viewPos - pos);
	vec3 L = normalize(lightPos - pos );
	vec3 N = normalize(normal);
	vec3 R = reflect(-L, N);

	//Difuse lighting
	float difusePower = dot( L, N);

	if( dot( V, N) < 0){
		difusePower = -difusePower;
	}
	//Distance calculation
	float dropOffDistance = 30;
	
	float lightDistance = length(lightPos - pos );
	difusePower *= dropOffFuction( lightDistance / dropOffDistance);

	//Specular lighing
	float spec = 0;
	float cosThetaSpec = dot( R, V);

	specPower *= dropOffFuction( lightDistance / dropOffDistance);

	if( cosThetaSpec > 0){
		spec = pow(cosThetaSpec, specPower);
	} 



	vec3 final = ambient + diffuse * difusePower + spec * specPower ;
	return final;
}

float dropOffFuction(float x){
	float dropFactor = 3;

	return 1 - pow(x,dropFactor);
}