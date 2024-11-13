#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform mat4 u_View;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    FragPos = vec3(u_Model * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(u_Model))) * normal;
    gl_Position = u_MVP * vec4(position, 1.0);
}

#shader fragment
#version 330 core

out vec4 color;

uniform vec4 u_Color;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float matShininess;

in vec3 FragPos;
in vec3 Normal;

void main()
{
    // Ambient
    vec3 ambient = lightAmbient * matAmbient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * (diff * matDiffuse);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    vec3 specular = lightSpecular * (spec * matSpecular);

    // Final color calculation
    vec3 result = (ambient + diffuse + specular) * u_Color.rgb; // Assuming u_Color is a vec4
    color = vec4(result, .5);  // Preserve u_Color's alpha
}