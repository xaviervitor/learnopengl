#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    
    float innerCutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

#define NR_POINT_LIGHTS 4

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Material material;
uniform vec3 viewPos;

out vec4 FragColor;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // All the colors source contributions are added in the color variable.
    // Apparently the way to calculate a dynamic number of PointLights
    // is to use the #define directive as a parameterized string and append
    // it to the shader string before compilation.
    vec3 color = CalcDirectionalLight(directionalLight, normal, viewDir);
    for (int i = 0; i < NR_POINT_LIGHTS ; i++) {
        color += CalcPointLight(pointLights[i], normal, FragPos, viewDir);
    }
    color += CalcSpotLight(spotLight, normal, FragPos, viewDir);

    FragColor = vec4(color, 1.0);
}

// The directional light is modeled after a infinetely distant (and powerful)
// source of light. The light is so distant that all the direction vectors are
// the same in all fragments.
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    // diffuse
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(normal, -lightDir), 0.0);
    
    // specular
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

    return ambient + diffuse + specular;
}

// The point light is modeled as a source of light that casts rays in all 
// directions, emitting from a point in space. Sources of light lose energy
// as they travel until they reach a surface. That's taken into account by
// the attenuation calculations.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ambient + diffuse + specular;
}

// The spot light is modeled after a source of light that has a direction and
// a cone of influence, like a flashlight. In the real world, lights don't
// behave like this, but this type of model is useful in computer graphics.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

    // flashlight cone
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = (light.innerCutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ambient + diffuse + specular;
}