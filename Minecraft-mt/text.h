#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include <map>
#include FT_FREETYPE_H

struct Character {
public:
	GLuint     TextureID;  // 字形纹理的ID
	glm::ivec2 Size;       // 字形大小
	glm::ivec2 Bearing;    // 从基准线到字形左部/顶部的偏移值
	GLuint     Advance;    // 原点距下一个字形原点的距离
};

class Font {
public:
	FT_Library ft;
	FT_Face face;
	std::map<GLchar, Character> Characters;
    GLuint VAO, VBO;
    GLuint Program;
    glm::mat4 projection;
	Font(std::string file, int size) {
		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		const char* f = file.data();
		if (FT_New_Face(ft, f, 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		FT_Set_Pixel_Sizes(face, 0, size);
		if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
	}
	void loadChar(int startID = 0, int endID = 128) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制
        for (GLubyte c = startID; c < endID; c++)
        {
            // 加载字符的字形 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // 生成纹理
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // 设置纹理选项
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // 储存字符供之后使用
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };
            Characters.insert(std::pair<GLchar, Character>(c, character));
        }
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
	}
    void render() {
        const std::string vs = "#version 330 core\n"
            "layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
            "out vec2 TexCoords;\n"
            "uniform mat4 projection;\n"
            "void main()\n"
            "{\n"
            "gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
            "TexCoords = vertex.zw;\n"
            "}\0";
        const std::string fs = "#version 330 core\n"
            "in vec2 TexCoords;\n"
            "out vec4 color;\n"
            "uniform sampler2D text;\n"
            "uniform vec3 textColor;\n"
            "void main()\n"
            "{\n"
            "vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
            "color = vec4(textColor, 1.0) * sampled;\n"
            "}\0";


        GLuint vsShader, fsShader;
        const char* vss = vs.c_str();
        const char* fss = fs.c_str();
        vsShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vsShader, 1, &vss, NULL);
        glCompileShader(vsShader);
        int  success;
        char infoLog[1024];
        glGetShaderiv(vsShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vsShader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        fsShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fsShader, 1, &fss, NULL);
        glCompileShader(fsShader);
        glGetShaderiv(fsShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fsShader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        Program = glCreateProgram();
        glAttachShader(Program, vsShader);
        glAttachShader(Program, fsShader);
        glLinkProgram(Program);
        glDeleteShader(vsShader);
        glDeleteShader(fsShader);
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, float width, float height)
    {
        glDisable(GL_DEPTH_TEST);
        projection = glm::ortho(0.0f, width, 0.0f, height);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // 激活对应的渲染状态
        glUseProgram(Program);
        glUniform3f(glGetUniformLocation(Program, "textColor"), color.x, color.y, color.z);
        glUniformMatrix4fv(glGetUniformLocation(Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        // 遍历文本中所有的字符
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = Characters[*c];

            GLfloat xpos = x + ch.Bearing.x * scale;
            GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            GLfloat w = ch.Size.x * scale;
            GLfloat h = ch.Size.y * scale;
            // 对每个字符更新VBO
            GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
            };
            // 在四边形上绘制字形纹理
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // 更新VBO内存的内容
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // 绘制四边形
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // 更新位置到下一个字形的原点，注意单位是1/64像素
            x += (ch.Advance >> 6) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glEnable(GL_DEPTH_TEST);
    }
};