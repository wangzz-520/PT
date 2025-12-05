#ifndef GRAYIMAGEGLWIDGET_H
#define GRAYIMAGEGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QImage>

// 继承QOpenGLFunctions_3_3_Core以直接调用OpenGL 3.3核心模式函数
class GrayImageGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit GrayImageGLWidget(QWidget *parent = nullptr);
    ~GrayImageGLWidget() override;

    // 加载8位无符号单通道灰度图（支持QImage或原始数据）
    bool loadGrayImage(const QImage& image);
    bool loadGrayImage(const uchar* data, int width, int height);

private:
    // QOpenGLWidget虚函数重写
    void initializeGL() override;  // 初始化OpenGL资源
    void resizeGL(int w, int h) override;  // 窗口大小变化时更新
    void paintGL() override;  // 渲染帧

    // 初始化着色器程序
    bool initShaders();
    // 初始化纹理（从灰度数据创建）
    bool initTexture(const uchar* data, int width, int height);

private:
    QOpenGLShaderProgram* m_shaderProgram;  // 着色器程序
    QOpenGLTexture* m_grayTexture;          // 灰度图纹理
    GLuint m_vao;                           // 顶点数组对象（VAO）
    GLuint m_vbo;                           // 顶点缓冲对象（VBO）
    int m_imageWidth = 0;                   // 图像宽度
    int m_imageHeight = 0;                  // 图像高度
};

#endif // GRAYIMAGEGLWIDGET_H
