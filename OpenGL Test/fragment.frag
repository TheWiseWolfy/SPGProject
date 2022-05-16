#version 400 core
										
out vec4 FragColor;
 
in vec3 outPoz;
in vec3 outColor;
in vec3 outNormal;

uniform vec3 lightPos;
uniform vec3 viewPos;

//uniform sampler2D ourTexture;

vec3 lighting(vec3 pos, vec3 normal, vec3 lightPos, vec3 viewPos, vec3 ambient, vec3 diffuse, vec3 specular, float specPower);

void main()
{
	vec3 ambient = vec3(0.2);
	vec3 diffuse = vec3(1, 1, 1);
	vec3 specular = vec3(0.2);
	float specPower = 0;

	vec3 color = lighting(outPoz, outNormal, lightPos, viewPos, ambient, diffuse, specular, specPower);

	//FragColor = mix(vec4(color, 1.0) , vec4(outColor, 1.0), 0.5 ) ;
	FragColor = vec4(outColor, 1.0);

}


vec3 lighting(vec3 pos, vec3 normal, vec3 lightPos, vec3 viewPos, vec3 ambient, vec3 diffuse, vec3 specular, float specPower)
{
	//functia calculeaza si returneaza culoarea conform cu modelul de iluminare Phong descris in documentatie
	// ...
	
	//Difuse lighting
	vec3 L = normalize(lightPos - pos );
	vec3 N = normalize(normal);
	float difusePower = 0;
	
	//if( dot( L, N) > 0){
		difusePower = dot( L, N);
	//}

	//Specular lighing

	vec3 V = normalize(viewPos - pos);
	vec3 R = reflect(-L, N);

	float spec = 0;
	float cosThetaSpec = dot( R, V);

	if( cosThetaSpec > 0){
		spec = pow(cosThetaSpec, specPower);
	} 

	vec3 final = ambient + diffuse * difusePower + spec * specPower ;
	return final;
}