#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

// Simple shader system for rendering
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec3 normal;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    out vec3 FragPos;
    out vec3 Normal;
    
    void main()
    {
        FragPos = vec3(model * vec4(position, 1.0));
        Normal = mat3(transpose(inverse(model))) * normal;
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 FragPos;
    in vec3 Normal;
    
    uniform vec3 objectColor;
    uniform vec3 lightPos;
    uniform vec3 viewPos;
    uniform vec3 lightColor;
    
    out vec4 FragColor;
    
    void main()
    {
        // Ambient
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;
        
        // Diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        
        // Specular
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = specularStrength * spec * lightColor;
        
        vec3 result = (ambient + diffuse + specular) * objectColor;
        FragColor = vec4(result, 1.0);
    }
)";

// Cube mesh data
class CubeMesh {
public:
    float vertices[216]; // 36 vertices * 6 floats (position + normal)
    unsigned int VAO, VBO;

    CubeMesh() {
        // Create cube vertices with normals
        setupCube();
        setupMesh();
    }

    void setupCube() {
        // Cube with normals
        float size = 0.5f;
        float cubeVertices[] = {
            // Front face
            -size, -size,  size,  0.0f,  0.0f,  1.0f,
             size, -size,  size,  0.0f,  0.0f,  1.0f,
             size,  size,  size,  0.0f,  0.0f,  1.0f,
             size,  size,  size,  0.0f,  0.0f,  1.0f,
            -size,  size,  size,  0.0f,  0.0f,  1.0f,
            -size, -size,  size,  0.0f,  0.0f,  1.0f,
            // Back face
            -size, -size, -size,  0.0f,  0.0f, -1.0f,
            -size,  size, -size,  0.0f,  0.0f, -1.0f,
             size,  size, -size,  0.0f,  0.0f, -1.0f,
             size,  size, -size,  0.0f,  0.0f, -1.0f,
             size, -size, -size,  0.0f,  0.0f, -1.0f,
            -size, -size, -size,  0.0f,  0.0f, -1.0f,
            // Left face
            -size,  size,  size, -1.0f,  0.0f,  0.0f,
            -size,  size, -size, -1.0f,  0.0f,  0.0f,
            -size, -size, -size, -1.0f,  0.0f,  0.0f,
            -size, -size, -size, -1.0f,  0.0f,  0.0f,
            -size, -size,  size, -1.0f,  0.0f,  0.0f,
            -size,  size,  size, -1.0f,  0.0f,  0.0f,
            // Right face
             size,  size,  size,  1.0f,  0.0f,  0.0f,
             size, -size, -size,  1.0f,  0.0f,  0.0f,
             size,  size, -size,  1.0f,  0.0f,  0.0f,
             size, -size, -size,  1.0f,  0.0f,  0.0f,
             size,  size,  size,  1.0f,  0.0f,  0.0f,
             size, -size,  size,  1.0f,  0.0f,  0.0f,
            // Bottom face
            -size, -size, -size,  0.0f, -1.0f,  0.0f,
             size, -size, -size,  0.0f, -1.0f,  0.0f,
             size, -size,  size,  0.0f, -1.0f,  0.0f,
             size, -size,  size,  0.0f, -1.0f,  0.0f,
            -size, -size,  size,  0.0f, -1.0f,  0.0f,
            -size, -size, -size,  0.0f, -1.0f,  0.0f,
            // Top face
            -size,  size, -size,  0.0f,  1.0f,  0.0f,
             size,  size,  size,  0.0f,  1.0f,  0.0f,
             size,  size, -size,  0.0f,  1.0f,  0.0f,
             size,  size,  size,  0.0f,  1.0f,  0.0f,
            -size,  size, -size,  0.0f,  1.0f,  0.0f,
            -size,  size,  size,  0.0f,  1.0f,  0.0f
        };
        memcpy(vertices, cubeVertices, sizeof(cubeVertices));
    }

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    ~CubeMesh() {
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }
};

// Scene object with transformation
struct SceneObject {
    std::string name;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    glm::vec3 color;
    CubeMesh* mesh;

    SceneObject(const std::string& n, glm::vec3 pos, glm::vec3 sc, glm::vec3 col)
        : name(n), position(pos), scale(sc), rotation(0.0f), color(col), mesh(nullptr) {}

    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
        return model;
    }
};

// Serialization for embedding
void serializeScene(const std::vector<SceneObject>& objects, const std::string& filename) {
    std::ofstream file(filename);
    file << "# Embedded Roblox Scene\n";
    file << "objects:\n";
    
    for (const auto& obj : objects) {
        file << "  - name: " << obj.name << "\n";
        file << "    position: [" << obj.position.x << ", " << obj.position.y << ", " << obj.position.z << "]\n";
        file << "    scale: [" << obj.scale.x << ", " << obj.scale.y << ", " << obj.scale.z << "]\n";
        file << "    color: [" << obj.color.x << ", " << obj.color.y << ", " << obj.color.z << "]\n";
    }
    file.close();
    std::cout << "Scene embedded to: " << filename << std::endl;
}

// 3D Viewer
class Viewer {
private:
    GLFWwindow* window;
    unsigned int shaderProgram;
    std::vector<SceneObject> objects;
    CubeMesh cubeMesh;
    glm::vec3 cameraPos;
    glm::vec3 cameraTarget;
    float rotationX, rotationY;

public:
    Viewer(int width, int height) : window(nullptr), rotationX(0), rotationY(0) {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW\n";
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, "Roblox 3D Viewer", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // V-sync

        glEnable(GL_DEPTH_TEST);

        compileShaders();
        cameraPos = glm::vec3(0.0f, 5.0f, 15.0f);
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void compileShaders() {
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);

        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cout << "Vertex shader compilation failed: " << infoLog << std::endl;
        }

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            std::cout << "Fragment shader compilation failed: " << infoLog << std::endl;
        }

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cout << "Shader program linking failed: " << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void addObject(const SceneObject& obj) {
        objects.push_back(obj);
    }

    void render() {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Camera
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // Lighting
        glm::vec3 lightPos(10.0f, 10.0f, 10.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
        GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
        GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
        GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
        glUniform3f(lightColorLoc, lightColor.x, lightColor.y, lightColor.z);

        // Render objects
        for (auto& obj : objects) {
            glm::mat4 model = obj.getModelMatrix();
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(colorLoc, obj.color.x, obj.color.y, obj.color.z);
            cubeMesh.draw();
        }

        glfwSwapBuffers(window);
    }

    void handleInput() {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            rotationY += 1.0f;
            updateCamera();
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            rotationX -= 1.0f;
            updateCamera();
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            rotationY -= 1.0f;
            updateCamera();
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            rotationX += 1.0f;
            updateCamera();
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            cameraPos.y += 0.1f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            cameraPos.y -= 0.1f;
        }
        // Embed scene with E key
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            static bool embedded = false;
            if (!embedded) {
                std::string filename = "embedded_scene_" + std::to_string((int)glfwGetTime()) + ".scene";
                serializeScene(objects, filename);
                embedded = true;
            }
        } else {
            // Reset E key state
        }
    }

    void updateCamera() {
        float radius = 15.0f;
        cameraPos.x = radius * glm::sin(glm::radians(rotationX)) * glm::cos(glm::radians(rotationY));
        cameraPos.z = radius * glm::cos(glm::radians(rotationX)) * glm::cos(glm::radians(rotationY));
        cameraPos.y = 5.0f + radius * glm::sin(glm::radians(rotationY));
    }

    bool isOpen() const {
        return window && !glfwWindowShouldClose(window);
    }

    void pollEvents() {
        glfwPollEvents();
    }

    ~Viewer() {
        glDeleteProgram(shaderProgram);
        if (window) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }
};

// Main demonstration
int main() {
    std::cout << "=== Roblox 3D Viewer (Press E to embed scene) ===" << std::endl;
    std::cout << "Controls:\n";
    std::cout << "  W/A/S/D - Rotate camera\n";
    std::cout << "  UP/DOWN - Move camera up/down\n";
    std::cout << "  E - Embed current scene as file\n";

    Viewer viewer(800, 600);

    // Add objects to scene
    SceneObject baseplate("Baseplate", glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(32.0f, 0.5f, 32.0f), glm::vec3(0.4f, 0.8f, 0.4f));
    viewer.addObject(baseplate);

    SceneObject brick1("Brick1", glm::vec3(-5.0f, 2.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.8f, 0.2f, 0.2f));
    viewer.addObject(brick1);

    SceneObject brick2("Brick2", glm::vec3(5.0f, 3.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.4f, 0.8f));
    viewer.addObject(brick2);

    SceneObject largeBrick("LargeBrick", glm::vec3(0.0f, 2.5f, -8.0f), glm::vec3(2.0f, 1.0f, 1.0f), glm::vec3(0.8f, 0.6f, 0.2f));
    viewer.addObject(largeBrick);

    // Render loop
    while (viewer.isOpen()) {
        viewer.pollEvents();
        viewer.handleInput();
        viewer.render();
    }

    return 0;
}
