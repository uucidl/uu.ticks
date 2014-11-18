#include <micros/api.h>

#include <GL/glew.h>

#include <vector>
#include <math.h>
#include <stdio.h> // for printf

#include <fstream>
#include <sstream>
#include <string>

extern void render_next_gl3(uint64_t time_micros)
{
        static struct Resources {
                GLuint shaders[2]      = {};
                GLuint shaderProgram   = 0;
                GLuint quadBuffers[2]  = {};
                GLuint quadVertexArray = 0;
                GLint indicesCount = 0;
        } all;

        // this incoming section initializes the static resources
        // necessary for drawing. It is executed only once!
        //
        // read the drawing code (after it) before checking the
        // initialization code.

        static bool mustInit = true;
        if (mustInit) {
                mustInit = false;

                // DATA
                auto dirname = [](std::string filepath) {
                        return filepath.substr(0, filepath.find_last_of("/\\"));
                };
                auto base_path = dirname(__FILE__);
                auto file_content = [base_path](std::string relpath) {
                        auto stream = std::ifstream(base_path + "/" + relpath);

                        if (stream.fail()) {
                                throw std::runtime_error("could not load file at " + relpath);
                        }

                        std::string content {
                                std::istreambuf_iterator<char>(stream),
                                std::istreambuf_iterator<char>()
                        };
                        return content;
                };

                char const* vertexShaderStrings[] = {
                        "#version 150\n",
                        "in vec4 position;\n",
                        "void main()\n",
                        "{\n",
                        "    gl_Position = position;\n",
                        "}\n",
                        nullptr,
                };

                std::string fragmentShaderContent = file_content("shader.fs");
                char const* fragmentShaderStrings[] = {
                        fragmentShaderContent.c_str(),
                        nullptr,
                };

                GLuint quadIndices[] = {
                        0, 1, 2, 2, 3, 0,
                };
                GLfloat quadVertices[] = {
                        -1.0, -1.0,
                        -1.0, +1.0,
                        +1.0, +1.0,
                        +1.0, -1.0,
                };

                // DATA -> OpenGL

                auto countStrings = [](char const* lineArray[]) -> GLint {
                        auto count = 0;
                        while (*lineArray++)
                        {
                                count++;
                        }
                        return count;
                };

                struct ShaderDef {
                        GLenum type;
                        char const** lines;
                        GLint lineCount;
                } shaderDefs[2] = {
                        { GL_VERTEX_SHADER, vertexShaderStrings, countStrings(vertexShaderStrings) },
                        { GL_FRAGMENT_SHADER, fragmentShaderStrings, countStrings(fragmentShaderStrings) },
                };
                {
                        auto i = 0;
                        all.shaderProgram  = glCreateProgram();

                        for (auto def : shaderDefs) {
                                GLuint shader = glCreateShader(def.type);
                                glShaderSource(shader, def.lineCount, def.lines, NULL);
                                glCompileShader(shader);
                                GLint status;
                                glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
                                if (status == GL_FALSE) {
                                        GLint length;
                                        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
                                        auto output = std::vector<char> {};
                                        output.reserve(length + 1);
                                        glGetShaderInfoLog(shader, length, &length, &output.front());
                                        fprintf(stderr, "ERROR compiling shader #%d: %s\n", 1+i, &output.front());
                                }
                                glAttachShader(all.shaderProgram, shader);
                                all.shaders[i++] = shader;
                        }
                        glLinkProgram(all.shaderProgram);
                }

                struct BufferDef {
                        GLenum target;
                        GLenum usage;
                        GLvoid const* data;
                        GLsizeiptr size;
                        GLint componentCount;
                        GLint shaderAttrib;
                } bufferDefs[] = {
                        { GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, quadIndices, sizeof quadIndices, 0, 0 },
                        { GL_ARRAY_BUFFER, GL_STATIC_DRAW, quadVertices, sizeof quadVertices, 2, glGetAttribLocation(all.shaderProgram, "position") },
                };

                glGenBuffers(sizeof bufferDefs / sizeof bufferDefs[0],
                             all.quadBuffers);
                {
                        auto i = 0;
                        for (auto def : bufferDefs) {
                                auto id = all.quadBuffers[i++];
                                glBindBuffer(def.target, id);
                                glBufferData(def.target, def.size, def.data, def.usage);
                                glBindBuffer(def.target, 0);
                        }
                }

                glGenVertexArrays(1, &all.quadVertexArray);
                glBindVertexArray(all.quadVertexArray);
                {
                        auto i = 0;
                        for (auto def : bufferDefs) {
                                auto id = all.quadBuffers[i++];
                                glBindBuffer(def.target, id);

                                if (def.target != GL_ARRAY_BUFFER) {
                                        continue;
                                }
                                glVertexAttribPointer(def.shaderAttrib, def.componentCount, GL_FLOAT,
                                                      GL_FALSE, 0, 0);
                                glEnableVertexAttribArray(def.shaderAttrib);
                        }
                }
                glBindVertexArray(0);
                all.indicesCount = sizeof quadIndices / sizeof quadIndices[0];
        }

        // Drawing code

        float const argb[4] = {
                0.0f, 0.39f, 0.19f, 0.29f,
        };
        glClearColor (argb[1], argb[2], argb[3], argb[0]);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glUseProgram(all.shaderProgram);
        {
                GLint viewport[4];
                glGetIntegerv(GL_VIEWPORT, viewport);
                GLfloat resolution[] = {
                        static_cast<GLfloat> (viewport[2]),
                        static_cast<GLfloat> (viewport[3]),
                        0.0,
                };
                glUniform3fv(glGetUniformLocation(all.shaderProgram, "iResolution"), 1,
                             resolution);

                auto globalTimeInSeconds = static_cast<GLfloat> ((double) time_micros / 1e6);
                glUniform1fv(glGetUniformLocation(all.shaderProgram, "iGlobalTime"), 1,
                             &globalTimeInSeconds);
        }
        glBindVertexArray(all.quadVertexArray);
        glDrawElements(GL_TRIANGLES, all.indicesCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glUseProgram(0);

}

extern void render_next_2chn_48khz_audio(uint64_t time_micros,
                int const sample_count, double left[/*sample_count*/],
                double right[/*sample_count*/])
{
        // silence
}

int main (int argc, char** argv)
{
        (void) argc;
        (void) argv;

        runtime_init();

        return 0;
}
