#include <X11/Xlib.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <unistd.h>
#include <math.h>

#define WIDTH  480
#define HEIGHT 800

const char *vs_src =
"attribute vec3 aPos;\n"
"uniform float uRot;\n"
"void main() {\n"
"  float c = cos(uRot);\n"
"  float s = sin(uRot);\n"
"  mat3 r = mat3(\n"
"    c, -s, 0.0,\n"
"    s,  c, 0.0,\n"
"    0.0,0.0,1.0);\n"
"  gl_Position = vec4(r * aPos, 1.0);\n"
"}\n";

const char *fs_src =
"precision mediump float;\n"
"void main() {\n"
"  gl_FragColor = vec4(1.0, 0.3, 0.2, 1.0);\n"
"}\n";

GLuint compile(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    return s;
}

int main() {
    Display *xd = XOpenDisplay(NULL);
    int scr = DefaultScreen(xd);

    Window win = XCreateSimpleWindow(
        xd, RootWindow(xd, scr),
        0, 0, WIDTH, HEIGHT, 0,
        BlackPixel(xd, scr),
        WhitePixel(xd, scr)
    );
    XMapWindow(xd, win);

    EGLDisplay ed = eglGetDisplay((EGLNativeDisplayType)xd);
    eglInitialize(ed, NULL, NULL);

    EGLint cfg_attr[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };

    EGLConfig cfg;
    EGLint n;
    eglChooseConfig(ed, cfg_attr, &cfg, 1, &n);

    EGLSurface surf = eglCreateWindowSurface(ed, cfg, (EGLNativeWindowType)win, NULL);
    EGLint ctx_attr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    EGLContext ctx = eglCreateContext(ed, cfg, EGL_NO_CONTEXT, ctx_attr);
    eglMakeCurrent(ed, surf, surf, ctx);

    glViewport(0, 0, WIDTH, HEIGHT);

    GLuint vs = compile(GL_VERTEX_SHADER, vs_src);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fs_src);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glUseProgram(prog);

    /* ACTUAL GEOMETRY */
    float verts[] = {
         0.0f,  0.6f, 0.0f,
        -0.6f, -0.6f, 0.0f,
         0.6f, -0.6f, 0.0f
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    GLint aPos = glGetAttribLocation(prog, "aPos");
    glEnableVertexAttribArray(aPos);
    glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLint uRot = glGetUniformLocation(prog, "uRot");

    float t = 0.0f;

    while (1) {
        t += 0.02f;

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1f(uRot, t);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        eglSwapBuffers(ed, surf);
        usleep(16000);
    }
}
