#include <iostream>
#include <vector>
#include <cmath>
#include <emscripten.h>

// Simple Vec3 for WebAssembly
struct Vec3 {
    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

// Physics Body for WebAssembly
struct PhysicsBody {
    Vec3 position;
    Vec3 velocity;
    Vec3 size;
    float mass;
    std::string type;
    std::string material;

    PhysicsBody(Vec3 pos, Vec3 sz, float m, std::string t, std::string mat)
        : position(pos), size(sz), mass(m), type(t), material(mat) {
        velocity = Vec3(0, 0, 0);
    }

    void integrate(float dt) {
        // Simple physics integration with improved damping
        float gravity = 98.1f;
        velocity.y -= gravity * dt;
        
        // Apply damping (air resistance)
        float damping = 0.99f;
        velocity.x *= damping;
        velocity.y *= damping;
        velocity.z *= damping;
        
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
        position.z += velocity.z * dt;

        // Ground collision with proper separation
        if (position.y - size.y/2 <= 0) {
            position.y = size.y/2;
            velocity.y = -velocity.y * 0.5f; // Bounce with damping
            
            // Apply friction when on ground
            velocity.x *= 0.95f;
            velocity.z *= 0.95f;
        }
    }
};

// Physics World
class PhysicsWorld {
private:
    std::vector<PhysicsBody> bodies;
    Vec3 gravity;

public:
    PhysicsWorld() : gravity(Vec3(0, -98.1f, 0)) {}

    int addBody(float px, float py, float pz,
                float sx, float sy, float sz,
                float mass, const char* type, const char* material) {
        Vec3 pos(px, py, pz);
        Vec3 size(sx, sy, sz);
        bodies.emplace_back(pos, size, mass, type, material);
        return bodies.size() - 1;
    }

    void simulate(float dt) {
        for (auto& body : bodies) {
            body.integrate(dt);
        }
    }

    void clear() {
        bodies.clear();
    }

    int getBodyCount() {
        return bodies.size();
    }

    // Get body data for JavaScript
    void getBodyData(int index, float* data) {
        if (index >= 0 && index < (int)bodies.size()) {
            const auto& body = bodies[index];
            data[0] = body.position.x;
            data[1] = body.position.y;
            data[2] = body.position.z;
            data[3] = body.size.x;
            data[4] = body.size.y;
            data[5] = body.size.z;
        }
    }
};

// Global physics world instance
PhysicsWorld* physicsWorld = nullptr;

// Emscripten bindings
extern "C" {

EMSCRIPTEN_KEEPALIVE
void initPhysics() {
    if (physicsWorld) delete physicsWorld;
    physicsWorld = new PhysicsWorld();
}

EMSCRIPTEN_KEEPALIVE
int addPhysicsBody(float px, float py, float pz,
                   float sx, float sy, float sz,
                   float mass, const char* type, const char* material) {
    if (!physicsWorld) return -1;
    return physicsWorld->addBody(px, py, pz, sx, sy, sz, mass, type, material);
}

EMSCRIPTEN_KEEPALIVE
void simulatePhysics(float dt) {
    if (physicsWorld) {
        physicsWorld->simulate(dt);
    }
}

EMSCRIPTEN_KEEPALIVE
void clearPhysicsWorld() {
    if (physicsWorld) {
        physicsWorld->clear();
    }
}

EMSCRIPTEN_KEEPALIVE
int getBodyCount() {
    return physicsWorld ? physicsWorld->getBodyCount() : 0;
}

EMSCRIPTEN_KEEPALIVE
void getBodyData(int index, float* data) {
    if (physicsWorld) {
        physicsWorld->getBodyData(index, data);
    }
}

EMSCRIPTEN_KEEPALIVE
void embedScene(const char* filename) {
    // This would save scene data - for now just log
    std::cout << "Embedding scene to: " << filename << std::endl;
}

}
