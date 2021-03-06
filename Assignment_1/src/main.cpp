//
// COMP 371 Labs Framework
//
//

#include <iostream>
#include <list>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <random>

#include "grid.hpp"
#include "shader.h"


using namespace glm;
using namespace std;

const unsigned int WIDTH = 1024;
const unsigned int HEIGHT = 768;

// Camera parameters for view transform
// make set Z parameter to 7.5 for camera pos. for 20x20 grid
vec3 cameraPosition(0.5f, 2.5f, 3.5f);
vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);

// Other camera parameters
float cameraSpeed = 1.0f;
float cameraFastSpeed = 2 * cameraSpeed;
float cameraHorizontalAngle = 90.0f;
float cameraVerticalAngle = -30.0f;
bool  cameraFirstPerson = true; // press 1 or 2 to toggle this variable

// Scaling
float scaleFactorBase = 1.0f;

// Translation
float translateX = 0.0f;
float translateZ = 0.0f;

//Rotation
float rotateBase = 0.0f;

float rotateX = 0.0f;
float rotateY = 0.0f;

float zoom = 45.0f;

// General Vector deallocation. 
template<typename T>
void resetToDefault(T &value)
{
    std::swap(T(), value);
}

// Vector Deallocation
void emptyGridVec()
{
    grid_vertices.clear();
    grid_vertices.shrink_to_fit();
}


std::vector<glm::vec3> gridGen()
{

	float i;
	float k;

    // 2.5 gives 20x20 grid, 12.5 gives 100x100 grid with square size of 0.5 units. inc by 0.5
    // replace values for i = -x; i <= x and in z and x pos in the vector for new grid dimensions

    for (GLfloat i = -100; i <= 100; i += 1) {
        grid_vertices.push_back(glm::vec3(i, 0, 100)); 
        grid_vertices.push_back(glm::vec3(i, 0, -100)); 
        
        grid_vertices.push_back(glm::vec3(100, 0, i)); 
        grid_vertices.push_back(glm::vec3(-100, 0, i)); 
        
    }
    return grid_vertices;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	
	// Translate model
	if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE) {
		translateX += 0.1f;
	}
	if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE) {
		translateX -= 0.1f;
	}
	if (key == GLFW_KEY_UP && action != GLFW_RELEASE) {
		translateZ -= 0.1f;
	}
	if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE) {
		translateZ += 0.1f;
	}
	// Show line view
	if (key == GLFW_KEY_L && action != GLFW_RELEASE) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	// Show vertex view
	if (key == GLFW_KEY_P && action != GLFW_RELEASE) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	// Show triangle view
	if (key == GLFW_KEY_O && action != GLFW_RELEASE) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	// Scale up
	if (key == GLFW_KEY_U && action != GLFW_RELEASE) {
		scaleFactorBase += 0.01f;

	}
	// Scale down
	if (key == GLFW_KEY_J && action != GLFW_RELEASE) {
        scaleFactorBase -= 0.01f;

	}

    // Rotate model right
    if (key == GLFW_KEY_E && action != GLFW_RELEASE) {
        rotateBase += 1.0f;
    }

    // Rotate model left
    if (key == GLFW_KEY_Q && action != GLFW_RELEASE) {
        rotateBase -= 1.0f;
    }

	//resets everything 
	if (key == GLFW_KEY_R && action != GLFW_RELEASE) {
        // Reset size
        scaleFactorBase = 1.0f;

        // Reset position
        translateX = 0.0f;
        translateZ = 0.0f;

        // Reset Angle
        rotateBase = 0.0f;

        // Reset Camera
        cameraPosition = glm::vec3(0.5f, 2.5f, 3.5f);
        cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
	}
	// Orient to above the y-axis
	if (key == GLFW_KEY_C && action != GLFW_RELEASE) {
        cameraPosition = glm::vec3(0.0f, 2.5f, 0.0f);
        cameraLookAt = glm::vec3(0.0f, -1.0f, 0.0f);
	}
	//go right (also face right, also cannot go past walls)
	if (key == GLFW_KEY_D && action != GLFW_RELEASE) {

	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void cursor_cb(GLFWwindow* window, double xpos, double ypos) {

	int pan = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	int zoom = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int tilt = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

	if (zoom == GLFW_PRESS) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		double xdiff = xpos - (width / 2.0f);
		double ydiff = ypos - (height / 2.0f);
		cameraPosition += cameraLookAt*(float)ydiff*-0.005f*0.04f;
		glfwSetCursorPos(window, width / 2.0f, height / 2.0f);
	}

	if (pan == GLFW_PRESS) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		double xdiff = xpos - (width / 2);
		double ydiff = ypos - (height / 2);

		glm::vec3 mod_v = glm::vec3(xdiff*0.005, 0, 0);
		glm::vec4 mod_v4 = glm::vec4(mod_v, 1);
		cameraLookAt = glm::normalize(cameraLookAt + glm::vec3(mod_v4 * 0.05f));
		glfwSetCursorPos(window, width / 2, height / 2);
	}

	if (tilt == GLFW_PRESS) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		double xdiff = xpos - (width / 2);
		double ydiff = ypos - (height / 2);

		glm::vec3 mod_v = glm::vec3(0, ydiff*-0.005, 0);
		glm::vec4 mod_v4 = glm::vec4(mod_v, 1);
		cameraLookAt = glm::normalize(cameraLookAt + glm::vec3(mod_v4 * 0.05f));
		glfwSetCursorPos(window, width / 2, height / 2);
	}
}

// Whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
        zoom -= (float)yoffset;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f; 
}

const char* getVertexShaderSource()
{
    return
                "#version 330 core\n"
                "layout (location = 0) in vec3 aPos;"
                "layout (location = 1) in vec3 aColor;"
                ""
                "uniform mat4 worldMatrix = mat4(1.0);"
                "uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
                "uniform mat4 projectionMatrix = mat4(1.0);"
                ""
                "out vec3 vertexColor;"
                "void main()"
                "{"
                "   vertexColor = aColor;"
                "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
                "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
                "}";
}


const char* getFragmentShaderSource()
{
    return
                "#version 330 core\n"
                "in vec3 vertexColor;"
                "out vec4 FragColor;"
                "uniform int fill;"
                "void main()"
                "{"
                "   FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);"
                "   if (fill == 0) {"
		                "FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);"
	                "}"
                "   if (fill == 1) {"
		                "FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);"
	                "}"
                "   if (fill == 2) {"
		                "FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);"
	                "}"
                 "   if (fill == 3) {"
		                "FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);"
	                "}"
                "   if (fill == 4) {"
		                "FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
	                "}"
                "   if (fill == 5) {"
		                "FragColor = vec4(0.63f, 0.32f, 0.18f, 1.0f);"
	                "}"
                "   if (fill == 6) {"
		                "FragColor = vec4(0.1f, 0.1f, 0.1f, 1.0f);"
	                "}"
                    
                    
                "}";
}

int compileAndLinkShaders()
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = getVertexShaderSource();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = getFragmentShaderSource();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}


int createVertexBufferObject()
{
    // Cube model
vec3 vertexArray[] = {  // position,                            color
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), //left - red
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
        
        vec3( 0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), // far - blue
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        
        vec3( 0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), // bottom - turquoise
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), // near - green
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        
        vec3( 0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        
        vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), // right - purple
        vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3( 0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        
        vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3( 0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
        
        vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), // top - yellow
        vec3( 0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
        
        vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f)
    };


    

    
    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    
    
    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
                          3,                   // size
                          GL_FLOAT,            // type
                          GL_FALSE,            // normalized?
                          2*sizeof(vec3), // stride - each vertex contain 2 vec3 (position, color)
                          (void*)0             // array buffer offset
                          );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          2*sizeof(vec3),
                          (void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
                          );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    
    return vertexBufferObject;
}




int main(int argc, char*argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();
    
#if defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // On windows, we set OpenGL version to 2.1, to support more hardware
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    // Create Window and rendering context using GLFW, resolution is 1024x768
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment - 1", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Compile and link shaders here ...
    int shaderProgram = compileAndLinkShaders();
    
    // We can set the shader once, since we have only one
    glUseProgram(shaderProgram);

    int width, height;
	glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    // Set projection matrix for shader, this won't change
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(zoom),(GLfloat) width / (GLfloat) height, 0.01f, 100.0f); 
    
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
                             cameraPosition + cameraLookAt,  // center
                             cameraUp );  // up
                             

    viewMatrix = glm::rotate(mat4(1.0f), glm::radians(rotateX), glm::vec3(0.0f,0.0f,-1.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotateY), glm::vec3(-1.0f, 0.0f, 0.0f));
    
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
    glfwSetCursorPosCallback(window, cursor_cb);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Enable Backface culling
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    
    
    // Entering Main Loop
    while(!glfwWindowShouldClose(window))
    {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;



        /********************* 
        GRID GENERATION 
        ***********************/
        gridGen();

        // Clear Buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       // GRID VAO

       // Gen VAO and VB
        GLuint grid_VAO, grid_VBO;
        glGenVertexArrays(1, &grid_VAO);
        glGenBuffers(1, &grid_VBO);

        // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
        glBindVertexArray(grid_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, grid_VBO);
        glBufferData(GL_ARRAY_BUFFER, grid_vertices.size() * sizeof(glm::vec3), &grid_vertices.front(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);


        GLuint fillLoc = glGetUniformLocation(shaderProgram, "fill");

        // Set cube color
        glUniform1i(fillLoc, 4);

        glm::vec3 triangleScale;
        glm::mat4 modelMatrix;
        triangleScale = glm::vec3(1.0f);
        modelMatrix = glm::mat4(1.0f);

        GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        
        
        // Draw grid
		glUniform1i(fillLoc, 0);
		glBindVertexArray(grid_VAO);
		
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glDrawArrays(GL_LINES, 0, 4 * grid_vertices.size());

		glBindVertexArray(0);
        // Vectors take up a lot of memory and was not being deallocated. This will deallocate the memory
        emptyGridVec();


        // Draw xyz axis
        int xAxisVbo, yAxisVbo, zAxisVbo;

        glUniform1i(fillLoc, 1);
        xAxisVbo = createVertexBufferObject();
        glBindBuffer(GL_ARRAY_BUFFER, xAxisVbo);

        mat4 xAxisCubeMatrix = translate(mat4(1.0f), vec3(2.5f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 0.01f, 0.01f));
        worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &xAxisCubeMatrix[0][0]);

        glDrawArrays(GL_LINES, 0, 12*3);


        glUniform1i(fillLoc, 2);
        yAxisVbo = createVertexBufferObject();
        glBindBuffer(GL_ARRAY_BUFFER, yAxisVbo);

        mat4 yAxisCubeMatrix = translate(mat4(1.0f), vec3(0.0f, 2.5f, 0.0f)) * scale(mat4(1.0f), vec3(0.01f, 5.0f, 0.01f));
        worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &yAxisCubeMatrix[0][0]);

        glDrawArrays(GL_LINES, 0, 12*3);

        glUniform1i(fillLoc, 3);
        zAxisVbo = createVertexBufferObject();
        glBindBuffer(GL_ARRAY_BUFFER, zAxisVbo);

        mat4 zAxisCubeMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 2.5f)) * scale(mat4(1.0f), vec3(0.01f, 0.01f, 5.0f));
        worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &zAxisCubeMatrix[0][0]);

        glDrawArrays(GL_LINES, 0, 12*3);

        glBindVertexArray(0);

        int vbo = createVertexBufferObject();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Hierarchical modeling
        // The part matrix will deal with each part of the letter Y (3 cubes in total)

        // Here are the matrices for handling the base (root) of Y
        glm::mat4 partMatrix;
        glm::mat4 groupMatrix = glm::translate(modelMatrix, glm::vec3(translateX, 0.0f, translateZ))
        * glm::rotate(modelMatrix, glm::radians(rotateBase) ,glm::vec3(0.0f, 1.0f, 0.0f))
        * glm::scale(modelMatrix, glm::vec3(scaleFactorBase, scaleFactorBase, scaleFactorBase));

        glm::mat4 scalingMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 1.0f, 0.2f));
        glm::mat4 translationMatrix = glm::translate(modelMatrix, vec3(-0.5f, 0.5f, 0.0f));

        partMatrix = translationMatrix * scalingMatrix;
        glm::mat4 worldMatrix = groupMatrix * partMatrix;

        // Make base cube for Y

        // Assign the color white for the Y model
        glUniform1i(fillLoc, 4);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);


        // Make left branch of Y 
        glm::mat4 rotationMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        scalingMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.5f, 0.2f));
        translationMatrix = glm::translate(modelMatrix, glm::vec3(-0.65f, 1.15f, 0.0f));
       
        partMatrix = translationMatrix * rotationMatrix * scalingMatrix;
        worldMatrix = groupMatrix * partMatrix;

        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);


        // Make right branch of Y
        rotationMatrix = glm::rotate(modelMatrix, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        translationMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f, 1.15f, 0.0f));

        partMatrix = translationMatrix * rotationMatrix * scalingMatrix;
        worldMatrix = groupMatrix * partMatrix;

        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);

        glBindVertexArray(0); 


        // Make 0 model, this is the base of the model
        int numberVbo = createVertexBufferObject();
        glBindBuffer(GL_ARRAY_BUFFER, numberVbo);

        glm::mat4 partMatrixNumber;
        glm::mat4 groupMatrixNumber = glm::translate(modelMatrix, glm::vec3(translateX, 0.0f, translateZ))
        *glm::rotate(modelMatrix, glm::radians(rotateBase), glm::vec3(0.0f, 1.0f, 0.0f))
        * glm::scale(modelMatrix, vec3(scaleFactorBase, scaleFactorBase, scaleFactorBase));

        glm::mat4 scalingMatrixNumber = glm::scale(modelMatrix, glm::vec3(0.5f, 0.2f, 0.2f));
        glm::mat4 translationMatrixNumber = glm::translate(modelMatrix, glm::vec3(0.5f, 0.1f, 0.0f));

        partMatrixNumber = translationMatrixNumber * scalingMatrixNumber;
        glm::mat4 worldMatrixNumber = groupMatrixNumber * partMatrixNumber;

        // Assign the color white for the zero model
        glUniform1i(fillLoc, 4);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrixNumber[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);


        // Make left side of zero model
        scalingMatrixNumber = glm::scale(modelMatrix, glm::vec3(0.2f, 1.0f, 0.2f));
        translationMatrixNumber = glm::translate(modelMatrix, glm::vec3(0.25f, 0.5f, 0.0f));

        partMatrixNumber = translationMatrixNumber * scalingMatrixNumber;
        worldMatrixNumber = groupMatrixNumber * partMatrixNumber;

        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrixNumber[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);


        // Make right side of zero model
        scalingMatrixNumber = glm::scale(modelMatrix, glm::vec3(0.2f, 1.0f, 0.2f));
        translationMatrixNumber = glm::translate(modelMatrix, glm::vec3(0.75f, 0.5f, 0.0f));

        partMatrixNumber = translationMatrixNumber * scalingMatrixNumber;
        worldMatrixNumber = groupMatrixNumber * partMatrixNumber;

        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrixNumber[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);

        
        //Make the top of the zero model
        scalingMatrixNumber = glm::scale(modelMatrix, glm::vec3(0.5f, 0.2f, 0.2f));
        translationMatrixNumber = glm::translate(modelMatrix, glm::vec3(0.5f, 0.9f, 0.0f));

        partMatrixNumber = translationMatrixNumber * scalingMatrixNumber;
        worldMatrixNumber = groupMatrixNumber * partMatrixNumber;

        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrixNumber[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);

        glBindVertexArray(0); 
        
        
        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // move camera down
        {
            cameraFirstPerson = true;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // move camera down
        {
            cameraFirstPerson = false;
        }


        // Camera controls
        bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;
        
        
        double mousePosX, mousePosY;

        glfwGetCursorPos(window, &mousePosX, &mousePosY);
        glfwSetKeyCallback(window, key_callback);
        
        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;
        
        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;
        
        // Convert to spherical coordinates
        const float cameraAngularSpeed = 60.0f;
        cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
        cameraVerticalAngle   -= dy * cameraAngularSpeed * dt;
        
        // Clamp vertical angle to [-85, 85] degrees
        cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
        if (cameraHorizontalAngle > 360)
        {
            cameraHorizontalAngle -= 360;
        }
        else if (cameraHorizontalAngle < -360)
        {
            cameraHorizontalAngle += 360;
        }
        
        float theta = radians(cameraHorizontalAngle);
        float phi = radians(cameraVerticalAngle);
        
        cameraLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));
        vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

        glm::normalize(cameraSideVector);
         

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
        {
            cameraPosition -= cameraSideVector * currentCameraSpeed * dt;
        }
        
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
        {
            cameraPosition += cameraSideVector * currentCameraSpeed * dt;
        }
        
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera up
        {
            cameraPosition -= cameraLookAt * currentCameraSpeed * dt;
        }
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera down
        {
            cameraPosition += cameraLookAt * currentCameraSpeed * dt;
        }

        mat4 viewMatrix = mat4(1.0);
        
        if(cameraFirstPerson) {
            viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
        }  else {
            float radius = 5.0f;
            glm::vec3 position = cameraPosition - radius * cameraLookAt;
            viewMatrix = lookAt(position, position + cameraLookAt, cameraUp);
            
        }
        

        GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    }

    
    // Shutdown GLFW
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    
	return 0;
}
