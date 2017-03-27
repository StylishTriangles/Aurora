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
    vec4 ambient = vec4(light.ambient, 0.333333333f) * texture2D(diffuseMap, TexCoord);

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(light.diffuse * diff, 0.333333333f) * texture2D(diffuseMap, TexCoord);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = 0.5*pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec4 specular = vec4(light.specular * spec, 0.333333333f) * texture2D(specularMap, TexCoord);

    color = ambient + diffuse + specular;
}
