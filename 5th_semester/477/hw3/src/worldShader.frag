#version 430

in Data
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
} data;

in vec3 LightVector;
in vec3 CameraVector;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

uniform sampler2D TexColor;
uniform sampler2D TexGrey;
uniform float textureOffset;

out vec4 FragColor;

vec3 ambientReflectenceCoefficient  = vec3(0.5f, 0.5f, 0.5f);
vec3 ambientLightColor              = vec3(0.6f, 0.6f, 0.6f);
vec3 diffuseReflectenceCoefficient  = vec3(1.0f, 1.0f, 1.0f);
vec3 diffuseLightColor              = vec3(1.0f, 1.0f, 1.0f);
vec3 specularReflectenceCoefficient = vec3(1.0f, 1.0f, 1.0f);
vec3 specularLightColor             = vec3(1.0f, 1.0f, 1.0f);
float SpecularExponent              = 10;



void main()
{
    vec3 fragmentNormal = normalize(data.Normal);
    vec3 lightVector    = normalize(lightPosition - data.Position);
    vec3 cameraVector   = normalize(cameraPosition - data.Position);
    vec3 halfVector     = normalize(lightVector + cameraVector);

    vec4 mappedTexColor = texture(TexColor, data.TexCoord);
    // diffuseReflectenceCoefficient = mappedTexColor.xyz;

    vec3 ambient  = ambientLightColor  * ambientReflectenceCoefficient;
    vec3 diffuse  = diffuseLightColor  * diffuseReflectenceCoefficient  * max(0.0f, dot(fragmentNormal, lightVector));
    vec3 specular = specularLightColor * specularReflectenceCoefficient * pow(max(0.0f, dot(fragmentNormal, halfVector)), SpecularExponent);

    FragColor = clamp(mappedTexColor * vec4(ambient + diffuse + specular, 1.0f), 0.0f, 1.0f);
}
