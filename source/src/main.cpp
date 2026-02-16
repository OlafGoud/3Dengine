#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include "shader.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const int GRID_SIZE = 20;

// ---------------- Globals ----------------
glm::vec3 cameraPos    = glm::vec3(8.0f, 8.0f, 8.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp     = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 projection;
glm::mat4 view;

std::vector<float> vertices; // x,y,z
std::vector<float> colors;   // r,g,b
std::vector<float> normals;  // nx,ny,nz

GLuint VAO, VBO, CBO, NBO;

// ---------------- Ray-Triangle Intersection ----------------
bool rayTriangleIntersect(
    const glm::vec3 &orig, const glm::vec3 &dir,
    const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
    float &t)
{
    const float EPSILON = 0.0000001f;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(dir, edge2);
    float a = glm::dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return false; // parallel
    float f = 1.0f / a;
    glm::vec3 s = orig - v0;
    float u = f * glm::dot(s, h);
    if (u < 0.0 || u > 1.0) return false;
    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(dir, q);
    if (v < 0.0 || u + v > 1.0) return false;
    t = f * glm::dot(edge2, q);
    if (t > EPSILON) return true;
    return false;
}

// ---------------- Mouse Callback ----------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        float x = (2.0f * xpos) / SCR_WIDTH - 1.0f;
        float y = 1.0f - (2.0f * ypos) / SCR_HEIGHT;

        glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);
        glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
        glm::vec3 ray_world = glm::normalize(glm::vec3(glm::inverse(view) * ray_eye));

        float closest_t = 10000.0f;
        int hitTri = -1;

        // Each triangle has 3 unique vertices now
        for (size_t i = 0; i < vertices.size(); i += 9)
        {
            glm::vec3 v0(vertices[i+0], vertices[i+1], vertices[i+2]);
            glm::vec3 v1(vertices[i+3], vertices[i+4], vertices[i+5]);
            glm::vec3 v2(vertices[i+6], vertices[i+7], vertices[i+8]);

            float t;
            if(rayTriangleIntersect(cameraPos, ray_world, v0, v1, v2, t))
            {
                if(t < closest_t)
                {
                    closest_t = t;
                    hitTri = (int)i;
                }
            }
        }

        if(hitTri != -1)
        {
            // Color the triangle red
            for(int j=0;j<3;j++)
            {
                colors[hitTri + j*3 + 0] = 1.0f;
                colors[hitTri + j*3 + 1] = 0.0f;
                colors[hitTri + j*3 + 2] = 0.0f;
            }

            // Update color buffer
            glBindBuffer(GL_ARRAY_BUFFER, CBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size()*sizeof(float), colors.data());
            std::cout << "Triangle clicked: vertices " << hitTri << " to " << hitTri+8 << "\n";
        }
    }
}

// ---------------- Main ----------------
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"Click Grid",nullptr,nullptr);
    if(!window){ std::cout<<"Failed\n"; glfwTerminate(); return -1;}
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){ std::cout<<"Failed GLAD\n"; return -1;}

    glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // ---------------- Generate Grid (Non-Shared Vertices) ----------------
    float half = GRID_SIZE / 2.0f;
    std::vector<glm::vec3> gridPositions;
    for(int z=0; z<=GRID_SIZE; z++){
        for(int x=0; x<=GRID_SIZE; x++){
            gridPositions.push_back(glm::vec3(x-half, (float)(rand()%10)/20.0f, z-half));
        }
    }

    for(int z=0; z<GRID_SIZE; z++){
        for(int x=0; x<GRID_SIZE; x++){
            int topLeft     =  z      * (GRID_SIZE + 1) + x;
            int topRight    =  topLeft + 1;
            int bottomLeft  = (z + 1) * (GRID_SIZE + 1) + x;
            int bottomRight =  bottomLeft + 1;

            // First triangle
            std::vector<glm::vec3> tri1 = { gridPositions[topLeft], gridPositions[bottomLeft], gridPositions[topRight] };
            glm::vec3 normal1 = glm::normalize(glm::cross(tri1[1]-tri1[0], tri1[2]-tri1[0]));
            for(auto &v : tri1){
                vertices.push_back(v.x); vertices.push_back(v.y); vertices.push_back(v.z);
                colors.push_back(1.0f); colors.push_back(1.0f); colors.push_back(1.0f);
                normals.push_back(normal1.x); normals.push_back(normal1.y); normals.push_back(normal1.z);
            }

            // Second triangle
            std::vector<glm::vec3> tri2 = { gridPositions[topRight], gridPositions[bottomLeft], gridPositions[bottomRight] };
            glm::vec3 normal2 = glm::normalize(glm::cross(tri2[1]-tri2[0], tri2[2]-tri2[0]));
            for(auto &v : tri2){
                vertices.push_back(v.x); vertices.push_back(v.y); vertices.push_back(v.z);
                colors.push_back(1.0f); colors.push_back(1.0f); colors.push_back(1.0f);
                normals.push_back(normal2.x); normals.push_back(normal2.y); normals.push_back(normal2.z);
            }
        }
    }

    // ---------------- Buffers ----------------
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&CBO);
    glGenBuffers(1,&NBO);

    glBindVertexArray(VAO);

    // Vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    // Colors
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(float), colors.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // ---------------- Shader ----------------
    Shader shader("bin/resources/shaders/shader.vs","bin/resources/shaders/shader.fs");
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    // ---------------- Main Loop ----------------
    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        projection = glm::perspective(glm::radians(45.0f),(float)SCR_WIDTH/(float)SCR_HEIGHT,0.1f,100.0f);

        shader.setMat4("uMVP", projection * view * model);
        shader.setMat4("uModel", model);

        // Lighting
        shader.setVec3("lightPos", glm::vec3(10.0f, 10.0f, 10.0f));
        shader.setVec3("viewPos", cameraPos);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size()/3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
