#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

// Models a spotlight, in this case a flashlight.
struct Light {
    vec3 position;
    vec3 direction;
    
    // Stores the cosine of the inner and outer cones.
    float innerCutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // Stores light attenuation values. These values are defined by the user.
    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

out vec4 FragColor;

void main() {
    // diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

    // cosine of the angle between current fragment lightDir and the defined
    // light direction of the flashlight.
    float theta = dot(lightDir, normalize(-light.direction));
    // Inside the inner cone (theta > inner) the light is at full power;
    // Outside the outer cone (theta < outer) the light is completely off;
    // Between the cones, the light intensity is affected by the
    // interpolation algorithm, (theta - outer) / (inner - outer) and
    // dims smoothly. All of this is done using the GLSL clamp() function.
    float epsilon = (light.innerCutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // Only diffuse and specular are affected, the ambient light is always
    // active, otherwise the rest of the scene would be completely unlit.
    diffuse *= intensity;
    specular *= intensity;

    // If the attenuation was only linear, it would not be realistic at
    // all. If the attenuation was only quadratic, the light would be
    // unbeliavable bright near it's source. This formula uses quadratic,
    // linear and constant values. This way, the attenuation will in be 
    // in linear decay close to the light and gradually in more quadratic 
    // decay as the distance grows. 
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}