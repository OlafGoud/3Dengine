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
glm::vec3 cameraPos   = glm::vec3(8.0f, 8.0f, 8.0f);
glm::vec3 cameraTarget= glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 projection;
glm::mat4 view;

std::vector<float> vertices;       // x,y,z
std::vector<unsigned int> indices; // triangles
std::vector<float> colors;         // r,g,b
GLuint VAO, VBO, EBO, CBO;

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

        // Test each triangle
        for (size_t i = 0; i < indices.size(); i += 3)
        {
            glm::vec3 v0(vertices[3*indices[i]+0], vertices[3*indices[i]+1], vertices[3*indices[i]+2]);
            glm::vec3 v1(vertices[3*indices[i+1]+0], vertices[3*indices[i+1]+1], vertices[3*indices[i+1]+2]);
            glm::vec3 v2(vertices[3*indices[i+2]+0], vertices[3*indices[i+2]+1], vertices[3*indices[i+2]+2]);

            float t;
            if(rayTriangleIntersect(cameraPos, ray_world, v0,v1,v2,t))
            {
                if(t < closest_t)
                {
                    closest_t = t;
                    hitTri = i;
                }
            }
        }

        if(hitTri != -1)
        {
            // Color the triangle red
            for(int j=0;j<3;j++)
            {
                colors[3*indices[hitTri+j]+0] = 1.0f;
                colors[3*indices[hitTri+j]+1] = 0.0f;
                colors[3*indices[hitTri+j]+2] = 0.0f;
            }

            // Update color buffer
            glBindBuffer(GL_ARRAY_BUFFER, CBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size()*sizeof(float), colors.data());
            std::cout << "Triangle clicked: vertices " << hitTri << " to " << hitTri+2 << "\n";
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

    // ---------------- Generate Grid ----------------
    float half = GRID_SIZE/2.0f;
    for(int z=0; z<=GRID_SIZE; z++){
        for(int x=0;x<=GRID_SIZE;x++){
            vertices.push_back(x-half);
            vertices.push_back((float)(rand()%10)/20);
            vertices.push_back(z-half);

            colors.push_back(1.0f);
            colors.push_back(1.0f);
            colors.push_back(1.0f);
        }
    }

    for(int z=0;z<GRID_SIZE;z++){
        for(int x=0;x<GRID_SIZE;x++){
            int topLeft     =  z      * (GRID_SIZE + 1) + x;
            int topRight    =  topLeft + 1;
            int bottomLeft  = (z + 1) * (GRID_SIZE + 1) + x;
            int bottomRight =  bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    std::vector<float> normals(vertices.size(), 0.0f);

    // After indices generation
    for(size_t i=0;i<indices.size();i+=3)
    {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i+1];
        unsigned int i2 = indices[i+2];

        glm::vec3 v0(vertices[3*i0], vertices[3*i0+1], vertices[3*i0+2]);
        glm::vec3 v1(vertices[3*i1], vertices[3*i1+1], vertices[3*i1+2]);
        glm::vec3 v2(vertices[3*i2], vertices[3*i2+1], vertices[3*i2+2]);

        glm::vec3 normal = glm::normalize(glm::cross(v1-v0, v2-v0));

        for(int j=0;j<3;j++){
            normals[3*indices[i+j]+0] = normal.x;
            normals[3*indices[i+j]+1] = normal.y;
            normals[3*indices[i+j]+2] = normal.z;
        }
    }

    // ---------------- Buffers ----------------
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    glGenBuffers(1,&CBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(float),vertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,CBO);
    glBufferData(GL_ARRAY_BUFFER,colors.size()*sizeof(float),colors.data(),GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(unsigned int),indices.data(),GL_STATIC_DRAW);

    glBindVertexArray(0);
    GLuint NBO;
    glGenBuffers(1, &NBO);
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(2);


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

        glm::mat4 mvp = projection * view * model;
        shader.setMat4("uMVP",mvp);
        shader.setMat4("uMVP", mvp);
        shader.setMat4("uModel", model);

        // Lighting
        shader.setVec3("lightPos", glm::vec3(10.0f, 10.0f, 10.0f));
        shader.setVec3("viewPos", cameraPos);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,(GLsizei)indices.size(),GL_UNSIGNED_INT,0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
