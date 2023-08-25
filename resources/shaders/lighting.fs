#version 330 core

// interpolated position and normal vector. will be used
// in the lighting calculations.
in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

void main() {
    // the ambient component represents all the bounce light an object
    // receives from the main light. in this case, it just adds a 
    // constant fraction of the light color on all the fragments.
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // the diffuse component represents the relation between the angle
    // the light hits the object. the smaller the angle between the
    // light vector and the surface normal, the brighter the light
    // should appear.
    vec3 normal = normalize(Normal);
    // lightPos - FragPos results in a negative light direction vector
    vec3 lightDir = normalize(lightPos - FragPos);
    // the dot product between the normal vector and the negative light
    // direction (both normalized) produces the angle of interest.
    // the light intensity should vary from 0 to 1 (colors are
    // always positive), so angles less than 0 are clamped.
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // the specular component represents the light that bounces directly
    // to the camera on reflective surfaces, like a mirror. 
    // the angle between the reflected light vector and the view vector
    // represent how reflective a fragment should be.
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    // the negative of lightDir (the original lightDir) has to be used
    // to get the reflection vector.
    vec3 reflectDir = reflect(-lightDir, normal);
    // the "32" is arbitrary. this intensity can be used to define an
    // object's reflective property 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (specular + ambient + diffuse) * objectColor;

    FragColor = vec4(result, 1.0);
}