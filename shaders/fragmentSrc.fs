#version 330 core

struct Material{
	vec3 Diffuse;
	vec3 Ambient;
	sampler2D Specular;
	float shininess;
};

struct DirLight{
	vec3 direction;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct PointLight{
	vec3 position;
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct SpotLight{
	vec3 spotDir;
	vec3 position;
	
	float cosPhi;
	float cosGamma;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 FragColor;

const float ambientStrength = 0.1;
const float specStrength = 0.9;

uniform Material material;
uniform sampler2D brickTex;
uniform vec3 viewPos;
uniform DirLight sun;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform uint toggleSpotLight;

vec3 viewDir;
vec4 objectColor;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos);

void main(){
viewDir = normalize(viewPos - fragPos);

objectColor = texture(brickTex, texCoord);

vec3 lighting = calcDirLight(sun, normal, viewDir);
lighting += calcPointLight(pointLight, normal, viewDir, fragPos);

if(toggleSpotLight == 1){
	lighting += calcSpotLight(spotLight, normal, viewDir, fragPos);
}

FragColor = vec4(lighting, 1.0) * objectColor;
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir){
	
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(-light.direction);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (diff * material.Diffuse) * light.diffuse;

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = (spec * vec3(texture(material.Specular, texCoord))) * light.specular;
	
	return (light.ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos){
	
	float d = distance(fragPos, light.position);
	float attenuation = 1.0 / (light.constant + light.linear * d + light.quadratic * pow(d, 2.0));
	
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (diff * material.Diffuse) * light.diffuse;

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = (spec * vec3(texture(material.Specular, texCoord))) * light.specular;
	
	return attenuation * (light.ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos){
	
	float d = distance(fragPos, light.position);
	float attenuation = 1.0 / (light.constant + light.linear * d + light.quadratic * pow(d, 2.0));
	
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);
	
	float cosTheta = dot(lightDir, normalize(-light.spotDir));
	if(cosTheta > light.cosPhi){
		
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = (diff * material.Diffuse) * light.diffuse;

		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = (spec * vec3(texture(material.Specular, texCoord))) * light.specular;
		
		float epsilon = light.cosPhi - light.cosGamma;
		float intensity = clamp((cosTheta - light.cosGamma) / epsilon, 0.0, 1.0);
		
		diffuse *= pow(intensity, 1.0);
		specular *= pow(intensity, 1.0);
		
		return attenuation * (light.ambient + diffuse + specular);

	}
	else return (attenuation * light.ambient);
}