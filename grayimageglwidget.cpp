#include "GrayImageGLWidget.h"
#include <QDebug>

GrayImageGLWidget::GrayImageGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_shaderProgram(nullptr)
    , m_grayTexture(nullptr)
    , m_vao(0)
    , m_vbo(0)
{
    // 设置OpenGL格式（核心模式3.3，避免兼容模式问题）
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);  // 抗锯齿（可选）
    setFormat(format);
}

GrayImageGLWidget::~GrayImageGLWidget()
{
    // 释放OpenGL资源（必须在当前上下文激活时）
    makeCurrent();
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    delete m_grayTexture;
    delete m_shaderProgram;
    doneCurrent();
}

// 加载QImage格式的灰度图（需确保是单通道8位）
bool GrayImageGLWidget::loadGrayImage(const QImage &image)
{
    if (image.isNull() || image.format() != QImage::Format_Grayscale8) {
        qWarning() << "无效的灰度图：必须是8位单通道格式";
        //return false;
    }
    // 转换为OpenGL纹理的行对齐（QImage默认是32位对齐，OpenGL默认4字节对齐，此处一致可直接用）
    QImage glImage = image.convertToFormat(QImage::Format_Grayscale8).mirrored(false, true);  // Y轴翻转（QImage原点在左上角，OpenGL在左下角）
    return loadGrayImage(glImage.bits(), glImage.width(), glImage.height());
}

// 加载原始8位无符号单通道灰度数据
bool GrayImageGLWidget::loadGrayImage(const uchar *data, int width, int height)
{
    if (!data || width <= 0 || height <= 0) {
        qWarning() << "无效的灰度数据：宽高必须为正，数据不为空";
        return false;
    }

    if(!context())
        return false;

    makeCurrent();  // 激活OpenGL上下文
    m_imageWidth = width;
    m_imageHeight = height;

    // 初始化或更新纹理
    if (m_grayTexture) {
        m_grayTexture->destroy();  // 销毁旧纹理
        delete m_grayTexture;
    }
    bool success = initTexture(data, width, height);
    doneCurrent();

    update();  // 触发重绘
    return success;
}

// 初始化OpenGL资源（着色器、VAO、VBO）
void GrayImageGLWidget::initializeGL()
{
    initializeOpenGLFunctions();  // 初始化OpenGL函数指针

    // 设置清屏颜色（黑色）
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 启用深度测试（可选，2D渲染可关闭）
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // 初始化着色器
    if (!initShaders()) {
        qFatal("着色器初始化失败！");
    }

    // 初始化全屏Quad的VAO和VBO（顶点包含位置和纹理坐标）
    // 顶点数据：(x, y, z) + (u, v)，覆盖整个标准化设备坐标（NDC）
    float vertices[] = {
        // 位置              纹理坐标
        1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  // 右上
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,  // 右下
       -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  // 左下
       -1.0f,  1.0f, 0.0f,  0.0f, 1.0f   // 左上
    };

    // 索引数据（四边形拆分为两个三角形）
    unsigned int indices[] = {
        0, 1, 3,  // 第一个三角形
        1, 2, 3   // 第二个三角形
    };

    // 创建VAO和VBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    GLuint ebo;
    glGenBuffers(1, &ebo);

    glBindVertexArray(m_vao);

    // 绑定VBO并上传顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 绑定EBO并上传索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 配置位置属性（location=0）
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 配置纹理坐标属性（location=1）
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 解绑VAO（重要：避免后续操作污染）
    glBindVertexArray(0);

    // EBO随VAO绑定，无需单独保留（VAO会记录EBO绑定状态）
    glDeleteBuffers(1, &ebo);
}

// 窗口大小变化时更新视口和投影（2D渲染无需投影矩阵，仅更新视口）
void GrayImageGLWidget::resizeGL(int w, int h)
{
    // 设置视口（将OpenGL渲染区域映射到Widget的像素坐标）
    glViewport(0, 0, w, h);
}

// 渲染帧
void GrayImageGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 清屏

    if (!m_shaderProgram || !m_grayTexture) {
        return;  // 资源未初始化，不渲染
    }

    // 激活着色器程序
    m_shaderProgram->bind();

    // 激活纹理单元0，并绑定灰度纹理
    glActiveTexture(GL_TEXTURE0);
    m_grayTexture->bind();

    // 告诉着色器：灰度纹理采样器使用纹理单元0
    m_shaderProgram->setUniformValue("u_grayTexture", 0);

    // 绑定VAO并绘制四边形（6个索引，2个三角形）
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // 解绑资源（可选，避免后续操作污染）
    glBindVertexArray(0);
    m_grayTexture->release();
    m_shaderProgram->release();
}

// 初始化着色器程序（顶点着色器+片段着色器）
bool GrayImageGLWidget::initShaders()
{
    m_shaderProgram = new QOpenGLShaderProgram(this);

    // 顶点着色器源码（将顶点位置直接输出为NDC，传递纹理坐标）
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;

        out vec2 TexCoord;

        void main() {
            gl_Position = vec4(aPos, 1.0);  // 2D渲染，直接使用NDC坐标
            TexCoord = aTexCoord;
        }
    )";

    // 片段着色器源码（读取纹理的单通道值，作为RGB灰度颜色）
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;

        uniform sampler2D u_grayTexture;  // 灰度纹理采样器

        void main() {
            float gray = texture(u_grayTexture, TexCoord).r;  // 读取红色通道（单通道纹理的r/g/b/a均为灰度值）
            FragColor = vec4(gray, gray, gray, 1.0);  // RGB均设为灰度值，alpha=1
        }
    )";

    // 编译顶点着色器
    if (!m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
        qWarning() << "顶点着色器编译失败：" << m_shaderProgram->log();
        return false;
    }

    // 编译片段着色器
    if (!m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)) {
        qWarning() << "片段着色器编译失败：" << m_shaderProgram->log();
        return false;
    }

    // 链接着色器程序
    if (!m_shaderProgram->link()) {
        qWarning() << "着色器链接失败：" << m_shaderProgram->log();
        return false;
    }

    return true;
}

// 从灰度数据创建OpenGL纹理
bool GrayImageGLWidget::initTexture(const uchar *data, int width, int height)
{
    m_grayTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_grayTexture->create();  // 创建纹理对象

    // 绑定纹理并设置参数
    m_grayTexture->bind();

    // 设置纹理内部格式（GL_R8：8位红色通道，对应单通道灰度）
    // 像素格式：GL_RED（输入数据是单通道红色），像素类型：GL_UNSIGNED_BYTE（8位无符号）
    glTexImage2D(
        GL_TEXTURE_2D,          // 纹理目标
        0,                      // 多级渐远纹理层级（0为基础层级）
        GL_R8,                  // 纹理内部存储格式
        width,                  // 纹理宽度
        height,                 // 纹理高度
        0,                      // 边框（必须为0）
        GL_RED,                 // 输入像素格式（单通道红色）
        GL_UNSIGNED_BYTE,       // 输入像素类型
        data                    // 灰度数据指针
    );

    // 生成多级渐远纹理（可选，提升缩放时的清晰度）
    m_grayTexture->generateMipMaps();

    // 设置纹理过滤模式（缩放时的采样方式）
    m_grayTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);  // 缩小：线性+多级渐远
    m_grayTexture->setMagnificationFilter(QOpenGLTexture::Linear);             // 放大：线性

    // 设置纹理环绕模式（超出纹理坐标范围时的处理）
    m_grayTexture->setWrapMode(QOpenGLTexture::ClampToEdge);  // 边缘拉伸（避免黑边）

    // 检查纹理创建错误
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        qWarning() << "纹理创建失败：OpenGL错误码" << error;
        return false;
    }

    m_grayTexture->release();  // 解绑纹理
    return true;
}
