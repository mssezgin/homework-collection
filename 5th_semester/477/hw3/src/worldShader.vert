#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTex;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 NormalMatrix;
uniform mat4 MVP;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

uniform sampler2D TexColor;
uniform sampler2D TexGrey;
uniform float textureOffset;

uniform float heightFactor;
uniform float imageWidth;
uniform float imageHeight;

out Data
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
} data;

out vec3 LightVector;  // Vector from Vertex to Light;
out vec3 CameraVector; // Vector from Vertex to Camera;



void main()
{
    vec4 modeledVertexPosition = ModelMatrix * vec4(VertexPosition, 1.0f);
    vec4 modeledVertexNormal = normalize(NormalMatrix * vec4(VertexNormal, 0.0f));
    vec4 mappedGreyColor = texture(TexGrey, vec2(VertexTex.s + textureOffset, VertexTex.t - textureOffset));

    modeledVertexPosition += mappedGreyColor.r * heightFactor * modeledVertexNormal;

    data.Position = modeledVertexPosition.xyz;
    data.Normal   = modeledVertexNormal.xyz;
    data.TexCoord = VertexTex;

    gl_Position = ProjectionMatrix * ViewMatrix * modeledVertexPosition;
}
