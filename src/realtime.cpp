#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDir>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"
#include "utils/transform.h"
#include "utils/debug.h"

using namespace Debug;

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

    // Animation buttons
    m_keyMap[Qt::Key_P]       = false;
    m_keyMap[Qt::Key_Left]    = false;
    m_keyMap[Qt::Key_Right]   = false;

    // Normal map button
    m_keyMap[Qt::Key_N]       = false;
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Delete VBOs and VAOs if scene exists
    if (m_scene.has_value()) m_scene->clean();

    // Delete shader programs
    for (GLuint m_shader: m_shaders) {
        glDeleteProgram(m_shader);
    }

    this->doneCurrent();
}

void Realtime::loadShaders() {
    // Fetch shader directory
    QDir dir("./resources/shaders");

    // Exit if shader directory does not exist or is empty
    if (!dir.exists() || dir.isEmpty()) {
        throw std::runtime_error("Invalid shader directory.");
    }

    // Fetch list of directory entries
    QFileInfoList fileInfoList = dir.entryInfoList();

    // Init temp shader file base name
    QString prevName = "";

    for (int i = 0; i < fileInfoList.size(); ++i) {
        const QFileInfo& fileInfo = fileInfoList.at(i);
        if (fileInfo.isFile()) {
            // Get current file base name
            QString currName = fileInfo.completeBaseName();

            // Skip if shader has been processed
            if (currName == prevName) continue;

            // Fetch full current path without file extension
            QString path = dir.filePath(currName);

            // Fetch shader paths
            QFileInfo vertexPath(path + ".vert");
            QFileInfo fragmentPath(path + ".frag");

            // Exit if vertex/fragment shader does not exist
            if (!vertexPath.exists()) {
                throw std::runtime_error("Missing vertex shader: " + vertexPath.filePath().toStdString());
            }
            if (!fragmentPath.exists()) {
                throw std::runtime_error("Missing fragment shader: " + fragmentPath.filePath().toStdString());
            }

            // Create shader program
            m_shaders.push_back(ShaderLoader::createShaderProgram(
                vertexPath.filePath().toStdString().c_str(),
                fragmentPath.filePath().toStdString().c_str()
            ));

            // Update processed shader name
            prevName = currName;
        }
    }

    glErrorCheck();
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

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    loadShaders();
}

void Realtime::paintGL() {
    if (!m_scene.has_value()) return;

    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (GLuint m_shader: m_shaders) {
        // Bind shader
        glUseProgram(m_shader);

        glErrorCheck();

        try {
            m_scene->draw(m_shader);
        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            finish();
        }

        glErrorCheck();

        // Unbind shader 
        glUseProgram(0);
    }
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Update projection matrix based on new aspect ratio if scene exists
    if (m_scene.has_value()) m_scene->resize(w, h);
}

void Realtime::sceneChanged() {
    makeCurrent();

    m_metaData = RenderData{};

    // Parse render data
    bool success = SceneParser::parse(settings.sceneFilePath, m_metaData.value());

    // Exit if parsing fails
    if (!success) {
        std::cerr << "Error loading scene: \"" << settings.sceneFilePath << "\"" << std::endl;
        finish();
        std::exit(EXIT_FAILURE);
    }

    // Free memory if scene exists
    if (m_scene.has_value()) m_scene->clean();

    // Create scene
    m_scene = Scene(m_metaData.value(),
                    size().width() * 1.f / size().height(),
                    settings.nearPlane,
                    settings.farPlane,
                    settings.shapeParameter1,
                    settings.shapeParameter2);

    glErrorCheck();

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    makeCurrent();

    if (!m_scene.has_value()) return;

    // Update projection matrix based on updated variables
    m_scene->updateProjection(settings.nearPlane, settings.farPlane);
    // Retessellate based on update variables
    m_scene->retessellate(settings.shapeParameter1, settings.shapeParameter2);

    update(); // asks for a PaintGL() call to occur
}

void Realtime::toggleFeatures() {
    // Play/pause animation
    if (m_keyMap[Qt::Key_P] && !m_pToggled) m_scene->playAnim();

    // Save toggle state to avoid per-frame checks
    m_pToggled = m_keyMap[Qt::Key_P];

    // Swap to previous animation if pressing left arrow, else to next if right arrow
    if (m_keyMap[Qt::Key_Left]) m_scene->swapAnim(false);
    else if (m_keyMap[Qt::Key_Right]) m_scene->swapAnim(true);

    // Toggle normal mapping
    if (m_keyMap[Qt::Key_N] && !m_nToggled) m_scene->toggleNormalMap();

    // Save toggle state to avoid per-frame checks
    m_nToggled = m_keyMap[Qt::Key_N];
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
    if (!m_metaData.has_value() || !m_scene.has_value()) return;

    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        glm::vec4 look = glm::normalize(m_metaData->cameraData.look);
        glm::vec4 up = glm::normalize(m_metaData->cameraData.up);

        float S = 0.001; // sensitivity

        // rotate left to right on y-axis
        glm::mat4 yaw = Transform::rotate(fmod(-deltaX * S, 2 * M_PI),
                                          {0.f, 1.f, 0.f});

        // rotate up and down on x-axis
        glm::mat4 pitch = Transform::rotate(fmod(deltaY * S, 2 * M_PI),
                                            glm::cross(glm::vec3{up}, glm::vec3{look}));

        m_metaData->cameraData.look = glm::normalize(yaw * pitch * look);
        m_metaData->cameraData.up = glm::normalize(yaw * pitch * up);

        m_scene->moveCam(m_metaData->cameraData.pos,
                         m_metaData->cameraData.look,
                         m_metaData->cameraData.up);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    if (!m_metaData.has_value() || !m_scene.has_value()) return;

    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    glm::vec4& pos = m_metaData->cameraData.pos;
    glm::vec4 look = glm::normalize(m_metaData->cameraData.look);
    glm::vec4 up = glm::normalize(m_metaData->cameraData.up);

    glm::vec4 left = glm::normalize(glm::vec4{glm::cross(glm::vec3{up}, glm::vec3{look}), 0.f});
    glm::vec4 y {0.f, 1.f, 0.f, 0.f};

    float m = 5.f * deltaTime; // scale with m = (m/s) * s

    if (m_keyMap[Qt::Key_W]) pos += look * m; // forward
    if (m_keyMap[Qt::Key_S]) pos -= look * m; // backward
    if (m_keyMap[Qt::Key_A]) pos += left * m; // strafe left
    if (m_keyMap[Qt::Key_D]) pos -= left * m; // strafe right
    if (m_keyMap[Qt::Key_Space]) pos += y * m; // jump
    if (m_keyMap[Qt::Key_Control]) pos -= y * m; // prone

    m_scene->moveCam(pos, m_metaData->cameraData.look, m_metaData->cameraData.up);

    // Update animation
    m_scene->updateAnim(deltaTime);

    // Toggle features
    toggleFeatures();

    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int fixedWidth = viewport[2];
    int fixedHeight = viewport[3];

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
    QImage flippedImage = image.flipped(Qt::Vertical); // Flip the image vertically

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
