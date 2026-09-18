#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform bool useTexture;
uniform vec3 solidColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 fogColor;

void main() {
    // ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 baseColor;
    if (useTexture) {
        baseColor = texture(texture_diffuse1, TexCoords).rgb;
    } else {
        baseColor = solidColor;
    }
    
    vec3 result = (ambient + diffuse + specular) * baseColor;
    
    // Calculate Fog
    float distance = length(viewPos - FragPos);
    float fogDensity = 0.008; // Decreased for deeper Line of Sight
    // GL_EXP2 fog math: f = e^(-(density * distance)^2)
    float fogFactor = exp(-pow(fogDensity * distance, 2.0));
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    // Mix the resulting color with the fog color
    result = mix(fogColor, result, fogFactor);

    FragColor = vec4(result, 1.0);
}
