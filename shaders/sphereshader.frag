#version 430 core
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 color;

uniform vec3 viewPos;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform float shininess;
uniform Light light;

void main()
{
//    color = texture2D(diffuseMap, TexCoord);
    // Ambient
    vec3 ambient = light.ambient * vec3(texture2D(diffuseMap, TexCoord));

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture2D(diffuseMap, TexCoord));

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.specular * spec * vec3(texture2D(specularMap, TexCoord));

    color = vec4(ambient + diffuse + specular, 1.0f);
}
