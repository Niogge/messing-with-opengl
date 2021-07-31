#version 330 core
out vec4 FragColor;


struct Material{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
};
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 
// struct PointLight {    
//     vec3 position;
    
//     float constant;
//     float linear;
//     float quadratic;  

//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };  
// #define NR_POINT_LIGHTS 4  


in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform Material material;
uniform vec3 viewPos;
uniform DirLight dirLight;
//uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcDirectionalLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
   
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = light.specular * spec * texture(material.texture_specular1, TexCoords).rrr;
    // combine results
    return (ambient + diffuse + specular);
}
// vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
// {
//     vec3 lightDir = normalize(light.position - fragPos);
//     // diffuse shading
//     float diff = max(dot(normal, lightDir), 0.0);
//     // specular shading
//     vec3 reflectDir = reflect(-lightDir, normal);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);

//     // attenuation
//     float distance    = length(light.position - fragPos);
//     float attenuation = 1.0 / (light.constant + light.linear * distance + 
//   			     light.quadratic * (distance * distance));    

//     // combine results
//     vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
//     vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
//     vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
//     ambient  *= attenuation;
//     diffuse  *= attenuation;
//     specular *= attenuation;
//     return (ambient + diffuse + specular);
// } 
void main()
{    
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 norm = 2.0 * texture2D (material.texture_normal1, TexCoords).rgb - 1.0;
    vec3 norm = normalize(Normal);
    vec3 result = CalcDirectionalLight(dirLight, norm, viewDir);
    // for(int i = 0; i < NR_POINT_LIGHTS; i++)
    //     result += CalcPointLight(pointLights[i], norm, FragPos, viewDir); 
    //FragColor = vec4(result,1.0);
    FragColor = vec4(result,1.0);
}