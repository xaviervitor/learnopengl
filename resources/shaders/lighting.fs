#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// interpolated position and normal vector. will be used
// in the lighting calculations.
in vec3 FragPos;
in vec3 Normal;

// object and light data are stored in Material and Light
// structs. 
uniform Material material;
uniform Light light;

uniform vec3 viewPos;

out vec4 FragColor;

void main() {
    // ambient
    // the previously used "ambient strength" is now defined as
    // the light.ambient component, as a full color.
    vec3 ambient = light.ambient * (material.ambient);

    // diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    // previously, light.diffuse was just 1.0f, and ommited.
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // the previously implemented "specular strength" is now defined 
    // as the light.specular component, as a full color.
    vec3 specular = light.specular * (spec * material.specular);

    // the final multiplication by the "object color" doesn't need to
    // be done anymore because all the color components of the material
    // are already multiplied in each lighting component. 
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}