#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

vec4 Inverse(){
    return vec4(vec3(1.0 - texture(screenTexture, TexCoords)),1.0);
}
vec4 GrayScale(){
    vec4 col = texture(screenTexture, TexCoords);
    float average = (col.r+col.g + col.b)/3.0;
    return vec4(average,average,average,1.0);
}
vec4 GrayScaleWeighted(){
    //our eye is more sensitive to green and least sensitive to blue
    //this is a more physical accurate grayscale
    vec4 col = texture(screenTexture, TexCoords);
    float average = 0.2126*col.r+0.7152*col.g + 0.0722* col.b;
    return vec4(average,average,average,1.0);
}

const float offset = 1.0 / 300.0;  

vec4 Kernels(){
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    //THE BRAIN
    // //Sharpen Kernel
    // float kernel[9] = float[](
    //     -1, -1, -1,
    //     -1,  9, -1,
    //     -1, -1, -1
    // );
    // //Blur Kernel (adapt value to increase or decrease blur )
    // float kernel[9] = float[](
    //     1.0 / 16, 2.0 / 16, 1.0 / 16,
    //     2.0 / 16, 4.0 / 16, 2.0 / 16,
    //     1.0 / 16, 2.0 / 16, 1.0 / 16  
    // );
    //Edge detection
    float kernel[9] = float[](
        1,  1, 1,
        1, -8, 1,
        1,  1, 1
    );
 

    //sample the texture 
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 res = vec3(0.0);
    //then using the logic inside the kernel, sum them 
    for(int i = 0; i < 9; i++)
            res += sampleTex[i] * kernel[i];

    return vec4(res, 1.0);
}
void main()
{ 
    // float depth = LinearizeDepth(gl_FragCoord.z) / far; 
    // FragColor = vec4(vec3(depth), 1.0);
    //FragColor = Inverse();
    //FragColor = GrayScale();
    //FragColor = GrayScaleWeighted();
    FragColor = Kernels();
}