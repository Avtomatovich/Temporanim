#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include <vector>

#include "shapes/cone.h"
#include "shapes/cube.h"
#include "shapes/sphere.h"
#include "shapes/cylinder.h"

#include "camera/camera.h"
#include "utils/sceneparser.h"

#include "physics/rigidbody.h"
#include <unordered_map>

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);
    bool sceneIsReady() const;

    void setPhysicsEnabled(bool enabled);
    void resetPhysics();
    void setRotationEnabled(bool enabled);
    void setBouncingEnabled(bool enabled);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    void shapeSetup();   // Builds VBO + VAO for the cube
    void VBOVAO(GLuint &vbo, GLuint &vao, std::vector<float> &data);

    void sendLights(GLuint shader);
    void sendGlobals(GLuint shader);
    void sendMaterial(GLuint shader, const SceneMaterial &mat);

    //proj 6
    void handleSphereBounce(RigidBody& rb);

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;
    GLuint m_shader = 0;

    bool isInitialized = false;

    Cube     m_cube;
    Sphere   m_sphere;
    Cylinder m_cylinder;
    Cone     m_cone;

    GLuint m_cube_vbo = 0,     m_cube_vao = 0;
    GLuint m_sphere_vbo = 0,   m_sphere_vao = 0;
    GLuint m_cylinder_vbo = 0, m_cylinder_vao = 0;
    GLuint m_cone_vbo = 0,     m_cone_vao = 0;

    std::vector<float> m_cubeData;
    std::vector<float> m_sphereData;
    std::vector<float> m_cylinderData;
    std::vector<float> m_coneData;

    Camera m_camera;
    RenderData m_renderData;

    //project 6
    std::unordered_map<int, RigidBody> m_rigidBodies;
    int m_nextRigidBodyId = 0;
    glm::vec3 m_gravity = glm::vec3(0, -9.8f, 0);
    bool m_physicsEnabled = false;
    bool m_rotationEnabled = false;
    bool m_bouncingEnabled = false;
    float m_groundY = -5.0f; // ground plane height
    // physics methods
    void initializePhysics();
    void updatePhysics(float deltaTime);
    void integrateRigidBody(RigidBody& rb, float dt);
    int createRigidBody(int shapeIndex, PrimitiveType type,const glm::mat4& ctm, float mass);
};
