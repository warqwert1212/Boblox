#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

// Simple Vec3 replacement (no GLM dependency)
struct Vec3 {
    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

// Shape definitions that can be embedded
struct Material {
    std::string name;
    Vec3 color;
    float roughness;
    float metallic;

    Material(const std::string& n = "default", Vec3 c = Vec3(0.8f, 0.8f, 0.8f), 
             float r = 0.5f, float m = 0.0f)
        : name(n), color(c), roughness(r), metallic(m) {}
};

struct Shape {
    std::string name;
    std::string type;  // "cube", "sphere", "cylinder", "wedge", "ball"
    Vec3 size;
    Vec3 position;
    Material material;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    Shape(const std::string& n, const std::string& t)
        : name(n), type(t), size(1.0f, 1.0f, 1.0f), position(0.0f, 0.0f, 0.0f) {}

    void generateCube() {
        vertices.clear();
        indices.clear();
        
        float sx = size.x / 2;
        float sy = size.y / 2;
        float sz = size.z / 2;

        // Front face
        vertices = {
            -sx, -sy,  sz,  // 0
             sx, -sy,  sz,  // 1
             sx,  sy,  sz,  // 2
            -sx,  sy,  sz,  // 3
            // Back face
            -sx, -sy, -sz,  // 4
             sx, -sy, -sz,  // 5
             sx,  sy, -sz,  // 6
            -sx,  sy, -sz   // 7
        };

        indices = {
            0, 1, 2, 2, 3, 0,  // Front
            4, 6, 5, 4, 7, 6,  // Back
            4, 0, 3, 3, 7, 4,  // Left
            1, 5, 6, 6, 2, 1,  // Right
            3, 2, 6, 6, 7, 3,  // Top
            4, 5, 1, 1, 0, 4   // Bottom
        };
    }

    void generateSphere(int segments = 32, int rings = 16) {
        vertices.clear();
        indices.clear();

        float radius = size.x / 2;
        const float PI_VAL = 3.14159265f;

        for (int i = 0; i <= rings; ++i) {
            float phi = PI_VAL * i / rings;
            for (int j = 0; j <= segments; ++j) {
                float theta = 2 * PI_VAL * j / segments;
                
                float x = radius * std::sin(phi) * std::cos(theta);
                float y = radius * std::cos(phi);
                float z = radius * std::sin(phi) * std::sin(theta);

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }

        for (int i = 0; i < rings; ++i) {
            for (int j = 0; j < segments; ++j) {
                int first = i * (segments + 1) + j;
                int second = first + segments + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }
    }

    void generateCylinder(int segments = 32, int height = 2) {
        vertices.clear();
        indices.clear();

        float radius = size.x / 2;
        float h = size.y;
        const float PI_VAL = 3.14159265f;

        // Top circle
        for (int i = 0; i <= segments; ++i) {
            float angle = 2 * PI_VAL * i / segments;
            float x = radius * std::cos(angle);
            float z = radius * std::sin(angle);
            
            vertices.push_back(x);
            vertices.push_back(h / 2);
            vertices.push_back(z);
        }

        // Bottom circle
        for (int i = 0; i <= segments; ++i) {
            float angle = 2 * PI_VAL * i / segments;
            float x = radius * std::cos(angle);
            float z = radius * std::sin(angle);
            
            vertices.push_back(x);
            vertices.push_back(-h / 2);
            vertices.push_back(z);
        }

        // Side faces
        for (int i = 0; i < segments; ++i) {
            int top1 = i;
            int top2 = i + 1;
            int bottom1 = segments + 1 + i;
            int bottom2 = segments + 1 + i + 1;

            indices.push_back(top1);
            indices.push_back(bottom1);
            indices.push_back(top2);

            indices.push_back(top2);
            indices.push_back(bottom1);
            indices.push_back(bottom2);
        }
    }

    void generateWedge() {
        vertices.clear();
        indices.clear();

        float sx = size.x / 2;
        float sy = size.y / 2;
        float sz = size.z / 2;

        vertices = {
            -sx, -sy,  sz,   // 0
             sx, -sy,  sz,   // 1
             sx, -sy, -sz,   // 2
            -sx, -sy, -sz,   // 3
            -sx,  sy, -sz,   // 4
             sx,  sy, -sz,   // 5
        };

        indices = {
            0, 1, 2, 2, 3, 0,  // Bottom
            3, 2, 5, 5, 4, 3,  // Top slope
            0, 4, 3, 0, 3, 1,  // Left
            1, 2, 5, 5, 1, 0   // Right
        };
    }

    void save(const std::string& filename) {
        std::ofstream file(filename);
        
        file << "{\n";
        file << "  \"shape\": \"" << name << "\",\n";
        file << "  \"type\": \"" << type << "\",\n";
        file << "  \"position\": [" << position.x << ", " << position.y << ", " << position.z << "],\n";
        file << "  \"size\": [" << size.x << ", " << size.y << ", " << size.z << "],\n";
        file << "  \"material\": {\n";
        file << "    \"name\": \"" << material.name << "\",\n";
        file << "    \"color\": [" << material.color.x << ", " << material.color.y << ", " << material.color.z << "],\n";
        file << "    \"roughness\": " << material.roughness << ",\n";
        file << "    \"metallic\": " << material.metallic << "\n";
        file << "  },\n";
        file << "  \"vertices\": " << vertices.size() / 3 << ",\n";
        file << "  \"indices\": " << indices.size() << "\n";
        file << "}\n";

        std::cout << "✓ Shape '" << name << "' embedded to: " << filename << std::endl;
    }

    void print() const {
        std::cout << "\nShape: " << name << std::endl;
        std::cout << "  Type: " << type << std::endl;
        std::cout << "  Position: [" << position.x << ", " << position.y << ", " << position.z << "]" << std::endl;
        std::cout << "  Size: [" << size.x << ", " << size.y << ", " << size.z << "]" << std::endl;
        std::cout << "  Material: " << material.name << std::endl;
        std::cout << "  Color: [" << material.color.x << ", " << material.color.y << ", " << material.color.z << "]" << std::endl;
        std::cout << "  Vertices: " << vertices.size() / 3 << std::endl;
        std::cout << "  Indices: " << indices.size() << std::endl;
    }
};

// Material library
class MaterialLibrary {
public:
    static Material getBrickRed() {
        return Material("BrickRed", Vec3(0.8f, 0.2f, 0.2f), 0.6f, 0.0f);
    }

    static Material getBrickBlue() {
        return Material("BrickBlue", Vec3(0.2f, 0.4f, 0.8f), 0.6f, 0.0f);
    }

    static Material getGold() {
        return Material("Gold", Vec3(1.0f, 0.85f, 0.0f), 0.2f, 0.8f);
    }

    static Material getPlastic() {
        return Material("Plastic", Vec3(0.5f, 0.5f, 0.5f), 0.7f, 0.0f);
    }

    static Material getSteel() {
        return Material("Steel", Vec3(0.7f, 0.7f, 0.75f), 0.3f, 0.8f);
    }

    static Material getWood() {
        return Material("Wood", Vec3(0.6f, 0.4f, 0.2f), 0.8f, 0.0f);
    }

    static Material getGlass() {
        return Material("Glass", Vec3(0.8f, 0.9f, 1.0f), 0.1f, 0.0f);
    }

    static Material getGreen() {
        return Material("Green", Vec3(0.2f, 0.7f, 0.2f), 0.6f, 0.0f);
    }
};

// Interactive shape builder
int main() {
    std::cout << "=== Roblox Shape Builder & Texture Creator ===\n" << std::endl;
    std::cout << "Commands:\n";
    std::cout << "  create <type> <name>  - Create a shape (cube, sphere, cylinder, wedge)\n";
    std::cout << "  size <shape> <x> <y> <z>  - Set shape size\n";
    std::cout << "  pos <shape> <x> <y> <z>   - Set position\n";
    std::cout << "  material <shape> <type>   - Set material (red, blue, gold, plastic, steel, wood, glass, green)\n";
    std::cout << "  show <shape>  - Display shape info\n";
    std::cout << "  embed <shape> - Save shape permanently\n";
    std::cout << "  list  - List all shapes\n";
    std::cout << "  exit  - Quit\n\n";

    std::vector<Shape> shapes;

    std::string command;
    while (true) {
        std::cout << "> ";
        std::cin >> command;

        if (command == "exit") {
            std::cout << "Saving all shapes...\n";
            break;
        }
        else if (command == "create") {
            std::string type, name;
            std::cin >> type >> name;

            Shape shape(name, type);

            if (type == "cube") {
                shape.generateCube();
                shape.material = MaterialLibrary::getPlastic();
            } else if (type == "sphere") {
                shape.generateSphere();
                shape.material = MaterialLibrary::getBrickBlue();
            } else if (type == "cylinder") {
                shape.generateCylinder();
                shape.material = MaterialLibrary::getSteel();
            } else if (type == "wedge") {
                shape.generateWedge();
                shape.material = MaterialLibrary::getWood();
            } else {
                std::cout << "Unknown shape type: " << type << std::endl;
                continue;
            }

            shapes.push_back(shape);
            std::cout << "✓ Created " << type << " '" << name << "'" << std::endl;
        }
        else if (command == "size") {
            std::string name;
            float x, y, z;
            std::cin >> name >> x >> y >> z;

            for (auto& shape : shapes) {
                if (shape.name == name) {
                    shape.size = Vec3(x, y, z);
                    if (shape.type == "cube") shape.generateCube();
                    else if (shape.type == "sphere") shape.generateSphere();
                    else if (shape.type == "cylinder") shape.generateCylinder();
                    else if (shape.type == "wedge") shape.generateWedge();
                    std::cout << "✓ Updated size for '" << name << "'" << std::endl;
                    break;
                }
            }
        }
        else if (command == "pos") {
            std::string name;
            float x, y, z;
            std::cin >> name >> x >> y >> z;

            for (auto& shape : shapes) {
                if (shape.name == name) {
                    shape.position = Vec3(x, y, z);
                    std::cout << "✓ Updated position for '" << name << "'" << std::endl;
                    break;
                }
            }
        }
        else if (command == "material") {
            std::string name, matType;
            std::cin >> name >> matType;

            for (auto& shape : shapes) {
                if (shape.name == name) {
                    if (matType == "red") shape.material = MaterialLibrary::getBrickRed();
                    else if (matType == "blue") shape.material = MaterialLibrary::getBrickBlue();
                    else if (matType == "gold") shape.material = MaterialLibrary::getGold();
                    else if (matType == "plastic") shape.material = MaterialLibrary::getPlastic();
                    else if (matType == "steel") shape.material = MaterialLibrary::getSteel();
                    else if (matType == "wood") shape.material = MaterialLibrary::getWood();
                    else if (matType == "glass") shape.material = MaterialLibrary::getGlass();
                    else if (matType == "green") shape.material = MaterialLibrary::getGreen();
                    else { std::cout << "Unknown material\n"; continue; }
                    std::cout << "✓ Applied material '" << matType << "' to '" << name << "'" << std::endl;
                    break;
                }
            }
        }
        else if (command == "show") {
            std::string name;
            std::cin >> name;

            for (const auto& shape : shapes) {
                if (shape.name == name) {
                    shape.print();
                    break;
                }
            }
        }
        else if (command == "embed") {
            std::string name;
            std::cin >> name;

            for (auto& shape : shapes) {
                if (shape.name == name) {
                    std::string filename = "embedded_" + name + ".shape";
                    shape.save(filename);
                    break;
                }
            }
        }
        else if (command == "list") {
            if (shapes.empty()) {
                std::cout << "No shapes created yet.\n";
            } else {
                std::cout << "\nCreated shapes:\n";
                for (size_t i = 0; i < shapes.size(); ++i) {
                    std::cout << i + 1 << ". " << shapes[i].name 
                              << " (" << shapes[i].type << ")"
                              << " - " << shapes[i].material.name << std::endl;
                }
            }
        }
        else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

    std::cout << "\nDone!\n";
    return 0;
}
