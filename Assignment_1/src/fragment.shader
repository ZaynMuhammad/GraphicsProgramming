#version 330 core\n
in vec3 vertexColor;
out vec4 FragColor;
uniform int fill;
void main()
{
   FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);
   if (fill == 0) {
        FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
    }
   if (fill == 1) {
        FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    }
   if (fill == 2) {
        FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
       if (fill == 3) {
        FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }
   if (fill == 4) {
        FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
   if (fill == 5) {
        FragColor = vec4(0.63f, 0.32f, 0.18f, 1.0f);
    }
   if (fill == 6) {
        FragColor = vec4(0.1f, 0.1f, 0.1f, 1.0f);
    }
    
    
};