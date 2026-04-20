#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

const float PI = 3.14159265f;
const float EPSILON = 0.0001f;

// Vector3D class for 3D physics calculations
class Vector3D {
public:
    float x, y, z;

    Vector3D(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

    Vector3D operator+(const Vector3D& v) const {
        return Vector3D(x + v.x, y + v.y, z + v.z);
    }

    Vector3D operator-(const Vector3D& v) const {
        return Vector3D(x - v.x, y - v.y, z - v.z);
    }

    Vector3D operator*(float scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    Vector3D operator/(float scalar) const {
        if (scalar != 0) return Vector3D(x / scalar, y / scalar, z / scalar);
        return Vector3D(0, 0, 0);
    }

    float dot(const Vector3D& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3D cross(const Vector3D& v) const {
        return Vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    float magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    float magnitudeSquared() const {
        return x * x + y * y + z * z;
    }

    Vector3D normalize() const {
        float mag = magnitude();
        if (mag > EPSILON) {
            return Vector3D(x / mag, y / mag, z / mag);
        }
        return Vector3D(0, 0, 0);
    }

    void print() const {
        printf("(%.2f, %.2f, %.2f)", x, y, z);
    }
};

// Quaternion for rotation
class Quaternion {
public:
    float x, y, z, w;

    Quaternion(float x = 0, float y = 0, float z = 0, float w = 1) 
        : x(x), y(y), z(z), w(w) {}

    Quaternion operator*(const Quaternion& q) const {
        return Quaternion(
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y - x * q.z + y * q.w + z * q.x,
            w * q.z + x * q.y - y * q.x + z * q.w,
            w * q.w - x * q.x - y * q.y - z * q.z
        );
    }

    Quaternion normalize() const {
        float mag = std::sqrt(x * x + y * y + z * z + w * w);
        if (mag > EPSILON) {
            return Quaternion(x / mag, y / mag, z / mag, w / mag);
        }
        return Quaternion(0, 0, 0, 1);
    }
};

// Part: Roblox-style part with physics
class Part {
public:
    std::string name;
    Vector3D position;
    Vector3D size;  // Width, Height, Depth
    Vector3D velocity;
    Vector3D angularVelocity;
    Vector3D acceleration;
    Quaternion rotation;
    float mass;
    float friction;
    float elasticity;
    bool anchored;
    Vector3D force;
    Vector3D torque;

    Part(std::string n, Vector3D pos, Vector3D sz, float m) 
        : name(n), position(pos), size(sz), mass(m), 
          friction(0.3f), elasticity(0.3f), anchored(false) {
        velocity = Vector3D(0, 0, 0);
        angularVelocity = Vector3D(0, 0, 0);
        acceleration = Vector3D(0, 0, 0);
        force = Vector3D(0, 0, 0);
        torque = Vector3D(0, 0, 0);
        rotation = Quaternion(0, 0, 0, 1);
    }

    float getVolume() const {
        return size.x * size.y * size.z;
    }

    // Simple bounding box
    bool isColliding(const Part& other) const {
        return (position.x - size.x / 2 < other.position.x + other.size.x / 2) &&
               (position.x + size.x / 2 > other.position.x - other.size.x / 2) &&
               (position.y - size.y / 2 < other.position.y + other.size.y / 2) &&
               (position.y + size.y / 2 > other.position.y - other.size.y / 2) &&
               (position.z - size.z / 2 < other.position.z + other.size.z / 2) &&
               (position.z + size.z / 2 > other.position.z - other.size.z / 2);
    }

    void applyForce(const Vector3D& f) {
        force = force + f;
    }

    void applyTorque(const Vector3D& t) {
        torque = torque + t;
    }

    void clearForces() {
        force = Vector3D(0, 0, 0);
        torque = Vector3D(0, 0, 0);
    }

    // Integrate physics (update position, velocity, rotation)
    void integrate(float dt) {
        if (anchored) return;

        // Linear motion: a = F/m
        acceleration = force / mass;
        velocity = velocity + acceleration * dt;
        
        // Apply friction
        float speed = velocity.magnitude();
        if (speed > EPSILON) {
            Vector3D frictionForce = velocity.normalize() * (-friction * mass * 9.81f);
            velocity = velocity + frictionForce * (dt / mass);
        }

        // Update position
        position = position + velocity * dt;

        // Angular motion (simplified)
        angularVelocity = angularVelocity + torque * (dt / (mass * 2.0f));
        
        // Apply angular damping
        angularVelocity = angularVelocity * 0.98f;

        clearForces();
    }

    void print() const {
        printf("%s:\n  Position: ", name.c_str());
        position.print();
        printf("\n  Velocity: ");
        velocity.print();
        printf("\n  Size: ");
        size.print();
        printf("\n  Mass: %.2f\n", mass);
    }
};

// Physics simulation engine (Roblox-style)
class PhysicsEngine {
private:
    std::vector<Part> parts;
    std::vector<Part*> workspace;
    Vector3D gravity;
    float groundLevel;

public:
    PhysicsEngine(Vector3D g = Vector3D(0, -9.81f * 10, 0), float ground = 0) 
        : gravity(g), groundLevel(ground) {}

    void addPart(const Part& part) {
        parts.push_back(part);
        workspace.push_back(&parts.back());
    }

    void handleCollisions() {
        // Simple ground collision
        for (auto& part : parts) {
            if (part.anchored) continue;

            // Bottom of part touches ground
            float bottomY = part.position.y - part.size.y / 2;
            if (bottomY <= groundLevel) {
                part.position.y = groundLevel + part.size.y / 2;
                
                // Bounce with elasticity
                part.velocity.y = -part.velocity.y * part.elasticity;
                
                // Stop small bounces
                if (std::abs(part.velocity.y) < 0.1f) {
                    part.velocity.y = 0;
                }
            }
        }

        // Part-to-part collisions
        for (size_t i = 0; i < parts.size(); ++i) {
            for (size_t j = i + 1; j < parts.size(); ++j) {
                if (parts[i].isColliding(parts[j])) {
                    handlePartCollision(parts[i], parts[j]);
                }
            }
        }
    }

    void handlePartCollision(Part& p1, Part& p2) {
        // Simple collision response
        Vector3D normal = p2.position - p1.position;
        normal = normal.normalize();

        // Relative velocity
        Vector3D relVel = p2.velocity - p1.velocity;
        float velAlongNormal = relVel.dot(normal);

        // Don't process if velocities are separating
        if (velAlongNormal >= 0) return;

        // Calculate impulse
        float avgElasticity = (p1.elasticity + p2.elasticity) / 2;
        float impulse = -(1 + avgElasticity) * velAlongNormal / (1 / p1.mass + 1 / p2.mass);

        // Apply impulse
        Vector3D impulseVector = normal * impulse;
        p1.velocity = p1.velocity - impulseVector * (1 / p1.mass);
        p2.velocity = p2.velocity + impulseVector * (1 / p2.mass);
    }

    void simulate(float dt) {
        // Apply gravity
        for (auto& part : parts) {
            if (!part.anchored) {
                part.applyForce(gravity * part.mass);
            }
        }

        // Integrate physics
        for (auto& part : parts) {
            part.integrate(dt);
        }

        // Handle collisions
        handleCollisions();
    }

    void printParts() const {
        for (size_t i = 0; i < parts.size(); ++i) {
            printf("Part %zu:\n", i);
            parts[i].print();
            printf("\n");
        }
    }

    std::vector<Part>& getParts() {
        return parts;
    }

    int getPartCount() const {
        return parts.size();
    }
};

// Main function demonstrating Roblox-style physics
int main() {
    printf("=== Roblox-Style Physics Engine (2008 Era) ===\n\n");

    // Create physics engine with gravity matching Roblox (higher than real gravity)
    PhysicsEngine engine(Vector3D(0, -98.1f, 0), 0);

    // Create baseplate (ground) - anchored
    Part baseplate("Baseplate", Vector3D(0, -1, 0), Vector3D(64, 1, 64), 10.0f);
    baseplate.anchored = true;
    engine.addPart(baseplate);

    // Create some falling parts
    Part brick1("Brick1", Vector3D(-5, 20, 0), Vector3D(2, 2, 2), 1.0f);
    brick1.velocity = Vector3D(5, 0, 0);
    brick1.elasticity = 0.5f;
    brick1.friction = 0.5f;
    engine.addPart(brick1);

    Part brick2("Brick2", Vector3D(5, 25, 0), Vector3D(2, 2, 2), 1.0f);
    brick2.velocity = Vector3D(-3, 0, 0);
    brick2.elasticity = 0.4f;
    brick2.friction = 0.4f;
    engine.addPart(brick2);

    Part largeBrick("LargeBrick", Vector3D(0, 15, -5), Vector3D(4, 2, 2), 2.0f);
    largeBrick.elasticity = 0.3f;
    largeBrick.friction = 0.6f;
    engine.addPart(largeBrick);

    // Simulation parameters
    float dt = 0.016f;  // ~60 FPS
    float totalTime = 0;
    int maxSteps = 2000;
    int printInterval = 100;

    printf("Initial state:\n");
    engine.printParts();
    printf("\n");

    // Run simulation
    for (int i = 0; i < maxSteps; ++i) {
        engine.simulate(dt);
        totalTime += dt;

        // Print every N steps
        if (i % printInterval == 0) {
            printf("=== Time: %.2fs (Step %d) ===\n", totalTime, i);
            engine.printParts();
            printf("\n");
        }

        // Stop if all parts have settled
        bool allSettled = true;
        for (auto& part : engine.getParts()) {
            if (std::abs(part.velocity.magnitude()) > 0.01f) {
                allSettled = false;
                break;
            }
        }
        if (allSettled && i > 100) {
            printf("All parts settled at %.2fs\n", totalTime);
            break;
        }
    }

    printf("\n=== Final state ===\n");
    engine.printParts();

    return 0;
}
