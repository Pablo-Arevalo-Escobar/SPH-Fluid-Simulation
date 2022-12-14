#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
#include<math.h>


#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "shader.h"
#include "camera.h"
#include "ParticleSystem.h"
#include "BoundingBox.h"
#include "ComputeShader.h"
#include "SPH.h"

 

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void computeShaderDebugData();


glm::mat4 lookAt(glm::vec4 position, glm::vec4 target, glm::vec4 up);

const unsigned int  WIDTH = 1280;//800;
const unsigned int  HEIGHT = 720;// 600;

bool update = false;
bool debug = false;
//Loop Control
float deltaTime = 0.0f;
float lastFrame = 0.0f;


//Process Input
bool switchCase = false;
float alpha = 0.2;
int direction = 0;

//Camera Control
Camera camera = Camera();
bool firstMouse = true;
float lastX = 400, lastY = 300;

int temp = 0;
const float fov = 90;




BoundingBox object(glm::vec3(0,-50,0), glm::vec3(1000, 400, 10));
static int objectXAxisPos = 0;
static int objectYAxisPos = -50;
static int objectZAxisPos = 0;



const float PARTICLE_SIZE = 0.5;

const int MAX_PARTICLE_COUNT = 100;
bool UI_MODE = true;

unsigned int BILLBOARD_VAO, pos_buff;
unsigned int rho_buff;


struct objectEmergeScene {
    glm::vec3 scale = glm::vec3(419, 153, 343);
    glm::vec3 pos = glm::vec3(4, -289, 46);
};

struct objectDamScene {
    glm::vec3 scale = glm::vec3(1000, 400, 10);
    glm::vec3 pos = glm::vec3(0, 86, -483);
};

struct objectDividerScene {
    glm::vec3 scale = glm::vec3(1000, 600, 10);
    glm::vec3 pos = glm::vec3(0, -50, 0);
};
static bool pause = true;

int main(void)
{


    //Instantiating the GLFW window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Creating a window object
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    //Initializing GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, WIDTH, HEIGHT);


    void framebuffer_size_callback(GLFWwindow * window, int width, int height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Initializing IMGUI instance
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();




    //Defining cube VAO
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    
    //SPH Simulation Parameters and Compute Shader Initialization
    SPH simulation;
    std::vector<glm::vec4>& positions = simulation.getPositions();
    std::vector<float>& densities = simulation.getDensities();
    glGenVertexArrays(1, &BILLBOARD_VAO);
    glBindVertexArray(BILLBOARD_VAO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, simulation.positionBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, simulation.colorBuffer);

    enum viewMode {WATER, NORMAL, DEPTH};
    viewMode particleRenderType = WATER;

    //Shader Declarations
    Shader shader("src/shader/shader.vert", "src/shader/shader.frag");
    Shader billboard_shader("src/shader/billboard/fluid/shader.vert", "src/shader/billboard/fluid/shader.frag");
    Shader externalObjectShader("src/shader/billboard/objects/shader.vert", "src/shader/billboard/objects/shader.frag");
    //Variable declarations
    glm::mat4 trans = glm::mat4(1.0f);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_demo_window = true;
    bool show_another_window = false;
    bool wireframe = switchCase;

    
    
    //OPENGL SETTINGS
    glEnable(GL_DEPTH_TEST);
    camera.Position = glm::vec3(-589.621, 317.928, 462.312);
    camera.Front = glm::vec3(0.721657, -0.335451, -0.605544);
    camera.Right = glm::vec3(0.642788, -0, 0.766044);
    camera.Up = glm::vec3(0.256971 ,0.942058, - 0.215624);
    camera.Yaw = -45;

    computeShaderDebugData();
   


    //Fixed time step
    int videoIndex = 0;
    double t = 0.0;
    double dt = 0.016;
    double currentTime = glfwGetTime();
    double accumulator = 0.0;

    //Visual Parameters
    float backgroundRValue = 233.0f;
    float backgroundGValue = 237.0f;
    float backgroundBValue = 238.0f;

    //Simulation parameters
    static int particleRenderMode = 0;
    static int viscocity          = simulation.VISC;
    static int gravity            = simulation.G;
    static float mass             = simulation.MASS;
    static float restitution      = simulation.RESTITUTION_COEFFICIENT;
    static float stiffnes         = simulation.K;
    static float restDensity      = simulation.REST_DENSITY;
    static int particleCountMult  = simulation.PARTICLE_COUNT_MULTIPLIER;
    static float smoothingRadius    = simulation.H;
    static int scene = 0;
    static bool barrier = false;

    //Object Paramaters
    static int objectXAxisScale = 1000;
    static int objectYAxisScale = 400;
    static int objectZAxisScale = 10;

    //Bounding Box Paramters
    bool showBoundingBox = true;
    static int boxXAxisScale = 800;
    static int boxYAxisScale = 150;
    static int boxZAxisScale = 800;

    

    while (!glfwWindowShouldClose(window))
    {
        //Update simulation parameters
        simulation.VISC = viscocity;
        simulation.G = gravity;
        simulation.MASS = mass;
        simulation.RESTITUTION_COEFFICIENT = restitution;
        simulation.K = stiffnes;
        simulation.REST_DENSITY = restDensity;
        simulation.PARTICLE_COUNT_MULTIPLIER = particleCountMult;
        simulation.updateSmoothingRadius(smoothingRadius);
        simulation.updateShaderUniforms();
        
        //Update Bounding Box parameters
        simulation.box.scale = glm::vec3(boxXAxisScale, boxYAxisScale, boxZAxisScale);
        simulation.box.position.y = boxYAxisScale / 2;

        //Update external object parameters
        glm::vec3 objScale(objectXAxisScale, objectYAxisScale, objectZAxisScale);
        glm::vec3 objPos(objectXAxisPos, objectYAxisPos, objectZAxisPos);
        object.position = objPos;
        object.scale = objScale;

        double newTime = glfwGetTime();
        double frameTime = newTime - currentTime;
        deltaTime = frameTime;
        if (frameTime > 0.25) frameTime = 0.25;

        currentTime = newTime;
        if (!pause) {
            if (barrier) { simulation.addExternalObject(object); }
            simulation.update();
        }
        processInput(window);
        glClearColor(backgroundRValue/ 255.0f, backgroundGValue/ 255.0f, backgroundBValue /255.0f,1.0f);
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       

        glBindVertexArray(VAO);
        glm::mat4 view = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f,10000.0f);

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setFloat("alpha", alpha);
        shader.setVec3("color", glm::vec3(232.0f / 255.0f, 66.0f / 255.0f, 53.0f / 255.0f));
        glBindVertexArray(VAO);
        //Draw The Bounding Box
        if (showBoundingBox) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, simulation.box.position);
            model = glm::scale(model, simulation.box.scale);
            shader.setMat4("model", model);
            glDrawArrays(GL_LINES, 0, 36);
        }

        //Draw externalobject
        if (barrier) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::mat4(1.0f);
            model = glm::translate(model, object.position);
            model = glm::scale(model, object.scale);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);


        //Run Compute Shader
        glBindVertexArray(BILLBOARD_VAO);
        //Draw the particles
        billboard_shader.use();
        view = camera.GetViewMatrix();
        billboard_shader.setFloat("BILLBOARD_SIZE", simulation.H/1.0f);
        billboard_shader.setMat4("view", view);
        billboard_shader.setMat4("projection", projection);
        billboard_shader.setInt("viewMode", particleRenderMode);
        glDrawArrays(GL_TRIANGLES, 0, 6*positions.size());
        glBindVertexArray(0);

        
    
        //UI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("UI");

        ImGui::ShowDemoWindow();
        if (ImGui::Button("START")) { pause = false; }
        ImGui::SameLine();
        if (ImGui::Button("PAUSE")) {pause = true;}

        if (ImGui::Button("RESET")) {
            if (scene == 2) { // Box emerge scene
                objectEmergeScene s;
                barrier = true;
                objectXAxisPos = s.pos.x;
                objectYAxisPos = s.pos.y;
                objectZAxisPos = s.pos.z;
                objectXAxisScale = s.scale.x;
                objectYAxisScale = s.scale.y;
                objectZAxisScale = s.scale.z;
            }
            else if (scene == 3) {
                objectDamScene s;
                barrier = true;
                objectXAxisPos = s.pos.x;
                objectYAxisPos = s.pos.y;
                objectZAxisPos = s.pos.z;
                objectXAxisScale = s.scale.x;
                objectYAxisScale = s.scale.y;
                objectZAxisScale = s.scale.z;
            }
            else if (scene == 4) {
                objectDividerScene s;
                barrier = true;
                objectXAxisPos = s.pos.x;
                objectYAxisPos = s.pos.y;
                objectZAxisPos = s.pos.z;
                objectXAxisScale = s.scale.x;
                objectYAxisScale = s.scale.y;
                objectZAxisScale = s.scale.z;
            }
            simulation.reset(scene);
        }

       

        //Text Info
        ImGui::Text("Number of particles = %d", particleCountMult * 1536);
        ImGui::Text("Smoothing Radius: %f", smoothingRadius);

        if (ImGui::CollapsingHeader("User guide")) {
            ImGui::Text("Press G to toggle the camera mouse movement");
            ImGui::Text("Use the arrow keys to move an object along the x and y axis");
            ImGui::Text("Use Q and E to move an object along the z axis");
        }
        
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
        ImGui::InputFloat("R", &backgroundRValue, 0, 255);
        ImGui::SameLine();
        ImGui::InputFloat("G", &backgroundGValue, 0, 255);
        ImGui::SameLine();
        ImGui::InputFloat("B", &backgroundBValue, 0, 255);



        //Simulation Parameters
        if (ImGui::CollapsingHeader("Simulation Parameters")) {
            const char* sceneValues[] = { "DAM BREAK", "WALL SLAM", "BOX EMERGE", "OBJECT WALL", "DIVIDER"};
            ImGui::Combo("Scene", &scene, sceneValues, IM_ARRAYSIZE(sceneValues));
            const char* renderMode[] = { "WATER", "NORMAL", "DEPTH" };
            ImGui::Combo("Particle Render Mode", &particleRenderMode, renderMode, IM_ARRAYSIZE(renderMode));
            ImGui::SliderInt("Particle Size Multiplier", &particleCountMult, 1, 100);
            ImGui::SliderInt("Dynamic Viscocity Coefficient", &viscocity, 10, 1000);
            ImGui::SliderInt("Gravity", &gravity, 0, -100);
            ImGui::SliderFloat("Mass", &mass, 1, 100);
            ImGui::SliderFloat("Coefficient of restitution", &restitution, 0, -1);
            ImGui::SliderFloat("Stiffness K", &stiffnes, 1, 3500);
            ImGui::SliderFloat("Rest Density", &restDensity, 100, 400);
        }

        //Bounding Box Parameters
        if (ImGui::CollapsingHeader("Bounding Box Parameters")) {
            if (ImGui::Button("Hide/Show Outline")) { showBoundingBox = !showBoundingBox; }
            ImGui::SliderInt("Box: x scale", &boxXAxisScale, 1, 10000);
            ImGui::SliderInt("Box: y scale", &boxYAxisScale, 1, 10000);
            ImGui::SliderInt("Box: z scale", &boxZAxisScale, 1, 10000);
            ImGui::InputInt("Box: y scale - precise", &boxYAxisScale, 1, 10000);

        }
        

        //Object Parameters
        if (ImGui::CollapsingHeader("Object Parameters")) {
            if (ImGui::Button("ADD BARRIER")) { barrier = true; }
            ImGui::SameLine();
            if (ImGui::Button("REMOVE BARRIER")) { barrier = false; }
            ImGui::SliderInt("Object: x scale", &objectXAxisScale, 1, 10000);
            ImGui::SliderInt("Object: y scale", &objectYAxisScale, 1, 10000);
            ImGui::SliderInt("Object: z scale", &objectZAxisScale, 1, 10000);
            ImGui::SliderInt("Object: x pos", &objectXAxisPos,-1000, 1000);
            ImGui::SliderInt("Object: y pos", &objectYAxisPos, -1000, 10000);
            ImGui::SliderInt("Object: z pos", &objectZAxisPos, -1000, 10000);
            
        }


        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


void computeShaderDebugData() {
    int max_compute_work_group_count[3];
    int max_compute_work_group_size[3];
    int max_compute_work_group_invocations;

    for (int idx = 0; idx < 3; idx++) {
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
    }
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);

    std::cout << "OpenGL Limitations: " << std::endl;
    std::cout << "maxmimum number of work groups in X dimension " << max_compute_work_group_count[0] << std::endl;
    std::cout << "maxmimum number of work groups in Y dimension " << max_compute_work_group_count[1] << std::endl;
    std::cout << "maxmimum number of work groups in Z dimension " << max_compute_work_group_count[2] << std::endl;

    std::cout << "maxmimum size of a work group in X dimension " << max_compute_work_group_size[0] << std::endl;
    std::cout << "maxmimum size of a work group in Y dimension " << max_compute_work_group_size[1] << std::endl;
    std::cout << "maxmimum size of a work group in Z dimension " << max_compute_work_group_size[2] << std::endl;

    std::cout << "Number of invocations in a single local work group that may be dispatched to a compute shader " << max_compute_work_group_invocations << std::endl;

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    int moveDelta = 1;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        objectYAxisPos += moveDelta;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        objectYAxisPos -= moveDelta;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        objectXAxisPos += moveDelta;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        objectXAxisPos -= moveDelta;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        pause = true;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        pause = false;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        objectZAxisPos += moveDelta;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        objectZAxisPos -= moveDelta;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        update = !update;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        UI_MODE = !UI_MODE;
        if (UI_MODE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }



    //CAMERA CONTROLS
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, 0.16);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, 0.16);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, 0.16);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, 0.16);
      
}
//double xpos, ypos; glfwGetCursorPos(window, &xpos, &ypos);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (UI_MODE){return;}
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


glm::mat4 lookAt(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
    //Form a vector from position to target
    glm::vec3 cameraDirection = glm::normalize(position - target);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, up));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));

    //Form a matrix composed of cameraDirection, cameraRight, cameraUp
    float axis[16] =
    {
        cameraRight.x, cameraRight.y, cameraRight.z, 0.0f,
        cameraUp.x, cameraUp.y, cameraUp.z, 0.0f,
        cameraDirection.x, cameraDirection.y, cameraDirection.z, 0.0f,
        0.0f,0.0f,0.0f,1.0f
    };
    glm::mat4 axisMatrix;
    memcpy(glm::value_ptr(axisMatrix), &axis , sizeof(axis));
    
    glm::mat4 positionMatrix(1.0f);
    positionMatrix[3][0] = position.x;
    positionMatrix[3][1] = position.y;
    positionMatrix[3][2] = position.z;
    return axisMatrix * positionMatrix;

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));

}


