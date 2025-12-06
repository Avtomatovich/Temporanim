#include "realtime.h"
#include "utils/shaderloader.h"


#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include <glm/gtx/transform.hpp>

static int lightTypeToInt(LightType t) {
    switch (t) {
    case LightType::LIGHT_POINT:       return 0;
    case LightType::LIGHT_DIRECTIONAL: return 1;
    case LightType::LIGHT_SPOT:        return 2;
    default:                           return 0;
    }
}

// ================== Rendering the Scene!

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    if (m_cube_vao)     glDeleteVertexArrays(1, &m_cube_vao);
    if (m_sphere_vao)   glDeleteVertexArrays(1, &m_sphere_vao);
    if (m_cylinder_vao) glDeleteVertexArrays(1, &m_cylinder_vao);
    if (m_cone_vao)     glDeleteVertexArrays(1, &m_cone_vao);

    if (m_cube_vbo)     glDeleteBuffers(1, &m_cube_vbo);
    if (m_sphere_vbo)   glDeleteBuffers(1, &m_sphere_vbo);
    if (m_cylinder_vbo) glDeleteBuffers(1, &m_cylinder_vbo);
    if (m_cone_vbo)     glDeleteBuffers(1, &m_cone_vbo);

    if (m_shader)       glDeleteProgram(m_shader);

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    std::cout << "[DEBUG] GLEW init complete" << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here

    glClearColor(0.f, 0.f, 0.f, 1.f);

    try {
        m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert",":/resources/shaders/default.frag");
    }
    catch (const std::runtime_error &e) {
        std::cerr << "Shader error: " << e.what() << std::endl;
        m_shader = 0;
    }

    shapeSetup();

    if (!settings.sceneFilePath.empty() &&
        SceneParser::parse(settings.sceneFilePath, m_renderData))
    {
        m_camera = Camera(m_renderData.cameraData,width()*m_devicePixelRatio,height()*m_devicePixelRatio);
    }
    else {
        SceneCameraData cam{};
        cam.pos = glm::vec4(0,0,5,1);
        cam.look = glm::vec4(0,0,-1,0);
        cam.up = glm::vec4(0,1,0,0);
        cam.heightAngle = glm::radians(45.f);
        m_camera = Camera(cam,width()*m_devicePixelRatio, height()*m_devicePixelRatio);
    }

    isInitialized = true;
}

bool Realtime::sceneIsReady() const {
    return isInitialized &&
           !m_renderData.shapes.empty();
}

void Realtime::VBOVAO(GLuint &vbo, GLuint &vao, std::vector<float> &data) {

    if (data.empty()) return;

    if (vao == 0) glGenVertexArrays(1, &vao);
    if (vbo == 0) glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float), data.data(),GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,6*sizeof(float), (void*)(3*sizeof(float)));

    glBindVertexArray(0);
}

void Realtime::shapeSetup()
{
    int p1 = settings.shapeParameter1;
    int p2 = settings.shapeParameter2;

    m_cube.updateParams(p1);
    m_cubeData = m_cube.getVertexData();
    VBOVAO(m_cube_vbo, m_cube_vao, m_cubeData);

    m_sphere.updateParams(p1, p2);
    m_sphereData = m_sphere.generateShape();
    VBOVAO(m_sphere_vbo, m_sphere_vao, m_sphereData);

    m_cylinder.updateParams(p1, p2);
    m_cylinderData = m_cylinder.generateShape();
    VBOVAO(m_cylinder_vbo, m_cylinder_vao, m_cylinderData);

    m_cone.updateParams(p1, p2);
    m_coneData = m_cone.generateShape();
    VBOVAO(m_cone_vbo, m_cone_vao, m_coneData);
}

void Realtime::sendGlobals(GLuint shader)
{
    const SceneGlobalData &g = m_renderData.globalData;

    glUniform1f(glGetUniformLocation(shader, "k_a"), g.ka);
    glUniform1f(glGetUniformLocation(shader, "k_d"), g.kd);
    glUniform1f(glGetUniformLocation(shader, "k_s"), g.ks);

    glm::vec3 pos = m_camera.getPosition();
    glUniform4f(glGetUniformLocation(shader, "cameraPos"),
                pos.x, pos.y, pos.z, 1.f);
}

void Realtime::sendMaterial(GLuint shader, const SceneMaterial &mat)
{
    glm::vec4 ambient  = mat.cAmbient;
    glm::vec4 diffuse  = mat.cDiffuse;
    glm::vec4 specular = mat.cSpecular;

    if (ambient == glm::vec4(0))  ambient = glm::vec4(0.2, 0.2, 0.2, 1);
    if (diffuse == glm::vec4(0))  diffuse = glm::vec4(0.8, 0.8, 0.8, 1);
    if (specular == glm::vec4(0)) specular = glm::vec4(0,0,0,1);

    glUniform4fv(glGetUniformLocation(shader, "cAmbient"),  1, &ambient[0]);
    glUniform4fv(glGetUniformLocation(shader, "cDiffuse"),  1, &diffuse[0]);
    glUniform4fv(glGetUniformLocation(shader, "cSpecular"), 1, &specular[0]);
    glUniform1f(glGetUniformLocation(shader, "shininess"), mat.shininess);
}

void Realtime::sendLights(GLuint shader)
{
    int n = std::min<int>(m_renderData.lights.size(), 8);
    glUniform1i(glGetUniformLocation(shader, "size"), n);

    for (int i = 0; i < n; i++) {
        const SceneLightData &L = m_renderData.lights[i];

        auto UL = [&](std::string s){
            return glGetUniformLocation(shader, s.c_str());
        };

        // Build correct GLSL-style array indexing: lightPos[0]
        std::string idx = "[" + std::to_string(i) + "]";

        glUniform1i(UL("lightType"      + idx), lightTypeToInt(L.type));
        glUniform4fv(UL("lightPos"      + idx), 1, &L.pos[0]);
        glUniform4fv(UL("lightDirections"      + idx), 1, &L.dir[0]);
        glUniform4fv(UL("lightColors"    + idx), 1, &L.color[0]);
        glUniform3fv(UL("function"      + idx), 1, &L.function[0]);
        glUniform1f(UL("lightAngle"     + idx),     L.angle);
        glUniform1f(UL("lightPenumbra"  + idx),     L.penumbra);
    }
}



void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    if (!sceneIsReady() || m_shader == 0) {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shader);

    glm::mat4 view = m_camera.getViewMatrix();
    glm::mat4 proj = m_camera.getProjMatrix();

    GLint locView = glGetUniformLocation(m_shader, "viewMatrix");
    GLint locProj = glGetUniformLocation(m_shader, "projMatrix");
    glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(locProj, 1, GL_FALSE, &proj[0][0]);

    sendGlobals(m_shader);
    sendLights(m_shader);

    GLint locModel = glGetUniformLocation(m_shader, "modelMatrix");

    for (size_t i = 0; i < m_renderData.shapes.size(); i++) {
        const RenderShapeData &shape = m_renderData.shapes[i];

        GLuint vao = 0;
        size_t nfloats = 0;

        switch (shape.primitive.type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            vao = m_cube_vao; nfloats = m_cubeData.size(); break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            vao = m_sphere_vao; nfloats = m_sphereData.size(); break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            vao = m_cylinder_vao; nfloats = m_cylinderData.size(); break;
        case PrimitiveType::PRIMITIVE_CONE:
            vao = m_cone_vao; nfloats = m_coneData.size(); break;
        default:
            continue;
        }

        if (vao == 0 || nfloats == 0)
            continue;

        sendMaterial(m_shader, shape.primitive.material);

        // physics transform if available
        glm::mat4 modelMatrix = shape.ctm;

        if ((m_physicsEnabled || m_rotationEnabled || m_bouncingEnabled)
            && m_rigidBodies.find(i) != m_rigidBodies.end()) {
            modelMatrix = m_rigidBodies[i].getTransformMatrix();
        }

        glUniformMatrix4fv(locModel, 1, GL_FALSE, &modelMatrix[0][0]);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, nfloats/6);
        glBindVertexArray(0);
    }
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    if (h>0) {
        // Students: anything requiring OpenGL calls when the program starts should be done here
        m_camera.setAspect(float(w)/float(h));
    }
}

void Realtime::sceneChanged() {
    if (!isInitialized) {
        return;
    }

    this->makeCurrent();

    if (SceneParser::parse(settings.sceneFilePath, m_renderData)) {
        m_camera = Camera(m_renderData.cameraData,
                          width()*m_devicePixelRatio,
                          height()*m_devicePixelRatio);
    }

    this->doneCurrent();

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    if (!isInitialized) {
        return;
    }

    this->makeCurrent();
    shapeSetup(); // update tessellation + buffers
    this->doneCurrent();
    update(); // asks for a PaintGL() call to occur
}

// ================== Physics
void Realtime::setPhysicsEnabled(bool enabled) {
    m_physicsEnabled = enabled;

    if (enabled && m_rigidBodies.empty()) {
        initializePhysics();
    }

    if (!enabled) {
        // reset to initial state
        for (auto& [id, rb] : m_rigidBodies) {
            rb.x = glm::vec3(rb.initialCTM[3]);

            glm::vec3 scale;
            scale.x = glm::length(glm::vec3(rb.initialCTM[0]));
            scale.y = glm::length(glm::vec3(rb.initialCTM[1]));
            scale.z = glm::length(glm::vec3(rb.initialCTM[2]));

            glm::mat3 rotMat;
            rotMat[0] = glm::vec3(rb.initialCTM[0]) / scale.x;
            rotMat[1] = glm::vec3(rb.initialCTM[1]) / scale.y;
            rotMat[2] = glm::vec3(rb.initialCTM[2]) / scale.z;

            rb.q = glm::quat_cast(rotMat);

            rb.P = glm::vec3(0);
            rb.L = glm::vec3(0);
            rb.computeAuxiliaryVariables();
        }
    }

    update();
}

void Realtime::setRotationEnabled(bool enabled) {
    m_rotationEnabled = enabled;
    update();
}

void Realtime::setBouncingEnabled(bool enabled) {
    m_bouncingEnabled = enabled;
    update();
}

void Realtime::resetPhysics() {
    for (auto& [id, rb] : m_rigidBodies) {
        rb.x = glm::vec3(rb.initialCTM[3]);

        glm::vec3 scale;
        scale.x = glm::length(glm::vec3(rb.initialCTM[0]));
        scale.y = glm::length(glm::vec3(rb.initialCTM[1]));
        scale.z = glm::length(glm::vec3(rb.initialCTM[2]));

        glm::mat3 rotMat;
        rotMat[0] = glm::vec3(rb.initialCTM[0]) / scale.x;
        rotMat[1] = glm::vec3(rb.initialCTM[1]) / scale.y;
        rotMat[2] = glm::vec3(rb.initialCTM[2]) / scale.z;

        rb.q = glm::quat_cast(rotMat);

        rb.P = glm::vec3(0);
        rb.L = glm::vec3(0);
        rb.force = glm::vec3(0);
        rb.torque = glm::vec3(0);
        rb.computeAuxiliaryVariables();
    }
    update();
}

void Realtime::initializePhysics() {
    m_rigidBodies.clear();
    m_nextRigidBodyId = 0;

    float lowestY = 0.0f;
    for (const auto& shape : m_renderData.shapes) {
        glm::vec3 pos = glm::vec3(shape.ctm[3]);
        lowestY = std::min(lowestY, pos.y);
    }
    m_groundY = lowestY - 3.0f;

    for (size_t i = 0; i < m_renderData.shapes.size(); i++) {
        const auto& shape = m_renderData.shapes[i];

        glm::vec3 pos = glm::vec3(shape.ctm[3]);

        float mass = 1.0f;
        int id = createRigidBody(i, shape.primitive.type, shape.ctm, mass);
    }
}

int Realtime::createRigidBody(int shapeIndex, PrimitiveType type,
                              const glm::mat4& ctm, float mass) {
    RigidBody rb(mass, ctm);
    rb.enableGravity = true;
    rb.isStatic = false;
    rb.shapeType = type;

    // scale from CTM to compute inertia tensor
    glm::vec3 scale;
    scale.x = glm::length(glm::vec3(ctm[0]));
    scale.y = glm::length(glm::vec3(ctm[1]));
    scale.z = glm::length(glm::vec3(ctm[2]));

    // inertia tensor based on shape type
    switch (type) {
    case PrimitiveType::PRIMITIVE_CUBE:
        rb.Ibody = RigidBody::computeBoxInertia(mass, scale);
        break;

    case PrimitiveType::PRIMITIVE_SPHERE:
        rb.Ibody = RigidBody::computeSphereInertia(mass, 0.5f * scale.x);
        break;

    case PrimitiveType::PRIMITIVE_CYLINDER:
        rb.Ibody = RigidBody::computeCylinderInertia(mass, 0.5f * scale.x, scale.y);
        break;

    case PrimitiveType::PRIMITIVE_CONE:
        rb.Ibody = RigidBody::computeConeInertia(mass, 0.5f * scale.x, scale.y);
        break;

    default:
        //sphere
        rb.Ibody = glm::mat3(1.0f);
        break;
    }

    // Precompute inverse
    rb.IbodyInv = glm::inverse(rb.Ibody);

    // Compute initial auxiliary variables
    rb.computeAuxiliaryVariables();

    // Store using shape index as key
    int id = shapeIndex;
    m_rigidBodies[id] = rb;

    return id;
}

void Realtime::handleSphereBounce(RigidBody& rb) {
    // sphere radius
    float radius = rb.initialScale.x * 0.5f;

    // if sphere hit ground
    float bottomY = rb.x.y - radius;

    if (bottomY <= m_groundY) {
        rb.x.y = m_groundY + radius;

        // bounciness
        float restitution = 0.7f;

        if (rb.v.y < 0) {
            rb.v.y = -rb.v.y * restitution;

            // new linear momentum to match new velocity
            rb.P = rb.mass * rb.v;

            // reduce horizontal velocity on impact
            rb.v.x *= 0.9f;
            rb.v.z *= 0.9f;
            rb.P = rb.mass * rb.v;

            // reduce spin on impact
            rb.L *= 0.95f;
        }
    }
}

void Realtime::integrateRigidBody(RigidBody& rb, float dt) {
    rb.computeAuxiliaryVariables();

    glm::vec3 x_dot = rb.v;
    glm::vec3 P_dot = rb.force;
    glm::vec3 L_dot = rb.torque;

    glm::quat omega_quat(0, rb.omega.x, rb.omega.y, rb.omega.z);
    glm::quat q_dot = 0.5f * (omega_quat * rb.q);

    // euler integration
    rb.x += x_dot * dt;
    rb.q += q_dot * dt;
    rb.q = glm::normalize(rb.q);
    rb.P += P_dot * dt;
    rb.L += L_dot * dt;

    //damping
    rb.P *= 0.99f;
    rb.L *= 0.99f;
}

void Realtime::updatePhysics(float deltaTime) {
    if (!m_physicsEnabled && !m_rotationEnabled && !m_bouncingEnabled) return;

    if ((m_physicsEnabled || m_rotationEnabled || m_bouncingEnabled) && m_rigidBodies.empty()) {
        initializePhysics();
    }

    for (auto& [id, rb] : m_rigidBodies) {
        rb.clearForces();
    }

    //  gravity if physics checkbox is enabled
    if (m_physicsEnabled) {
        for (auto& [id, rb] : m_rigidBodies) {
            if (!rb.isStatic && rb.enableGravity) {
                rb.applyForce(rb.mass * m_gravity);
            }
        }
    }

    // torque if rotation is enabled
    if (m_rotationEnabled) {
        for (auto& [id, rb] : m_rigidBodies) {
            if (!rb.isStatic) {
                glm::vec3 axis = glm::normalize(glm::vec3(
                    std::sin(id * 1.23f),
                    std::cos(id * 4.56f),
                    std::sin(id * 7.89f)
                    ));
                rb.torque += axis * 3.0f;
            }
        }
    }

    for (auto& [id, rb] : m_rigidBodies) {
        if (!rb.isStatic) {
            integrateRigidBody(rb, deltaTime);
        }
    }

    // bouncing
    if (m_bouncingEnabled) {
        for (auto& [id, rb] : m_rigidBodies) {
            // spheres bounce
            if (!rb.isStatic && rb.shapeType == PrimitiveType::PRIMITIVE_SPHERE) {
                handleSphereBounce(rb);
            }
        }
    }
}


// ================== Camera Movement!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();

        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        m_camera.rotate(deltaX, deltaY);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    (void)event;
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    float speed = 5.f;

    if (m_keyMap[Qt::Key_W])       m_camera.moveForward( speed * deltaTime);
    if (m_keyMap[Qt::Key_S])       m_camera.moveForward(-speed * deltaTime);
    if (m_keyMap[Qt::Key_D])       m_camera.moveRight  ( speed * deltaTime);
    if (m_keyMap[Qt::Key_A])       m_camera.moveRight  (-speed * deltaTime);
    if (m_keyMap[Qt::Key_Space])   m_camera.moveUp     ( speed * deltaTime);
    if (m_keyMap[Qt::Key_Control]) m_camera.moveUp     (-speed * deltaTime);

    updatePhysics(deltaTime);

    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
