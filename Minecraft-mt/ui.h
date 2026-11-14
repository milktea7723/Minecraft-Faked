#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>

class UI {
public:
	float x, y, z;
	float width, height;
	std::vector<float> vertices;
	std::string vShaderSource, fShaderSource;
	unsigned int VAO, VBO;
	unsigned int vShader, fShader, Program;
	unsigned int texture;
	float xoffset, yoffset, textureWidth, textureHeight;
	UI() {
		vShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec2 aTexCoord;\n"
		"out vec2 TexCoord;\n"
		"uniform mat4 model;\n"
		"uniform mat4 projection;\n"
		"void main(){\n"
		"    gl_Position = projection * model * vec4(aPos, 1.0);\n"
		"    TexCoord = aTexCoord;\n"
		"}\0";
		fShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoord;\n"
		"uniform sampler2D ourTexture;\n"
		"void main(){\n"
		"    FragColor = texture(ourTexture, TexCoord);\n"
		"}\0";
	}
	void Generate() {

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glGenTextures(1, &texture);
	}
	void setShader(std::string vertexShaderSource = "", std::string fragmentShaderSource = "", bool change = false) {
		if (change) {
			vShaderSource = vertexShaderSource;
			fShaderSource = fragmentShaderSource;
		}
		const char* vshadersource = vShaderSource.data();
		const char* fshadersource = fShaderSource.data();
		vShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vShader, 1, &vshadersource, NULL);
		glCompileShader(vShader);
		fShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fShader, 1, &fshadersource, NULL);
		glCompileShader(fShader);
		Program = glCreateProgram();
		glAttachShader(Program, vShader);
		glAttachShader(Program, fShader);
		glLinkProgram(Program);
		glDeleteShader(vShader);
		glDeleteShader(fShader);
	}
	void setVertices(float px, float py, float pz, float Width, float Height, float XOffset, float YOffset, float TextureWidth, float TextureHeight) {
		x = px;y = py;z = pz;
		width = Width;height = Height;
		xoffset = XOffset;yoffset = YOffset;textureWidth = TextureWidth;textureHeight = TextureHeight;
		updateVertices();
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindVertexArray(VAO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	void setTexture(std::string file) {
		int width, height, nrChannel;
		const char* fileRoad = file.data();
		unsigned char* data = stbi_load(fileRoad , &width, &height, &nrChannel, 4);
		if (!data) {
			std::cout << "WARNING::WARNING::TEXTURE WAS NOT LOADED::WARNING::WARNING\n";
			const char* reason = stbi_failure_reason();
			std::cout << "Error Reason:" << reason << "\n";
		}
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	void draw(glm::mat4 ortho, glm::mat4 model) {
		glUseProgram(Program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		int modelLoc = glGetUniformLocation(Program, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int projectionLoc = glGetUniformLocation(Program, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(ortho));
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 5);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}
	void updateVertices() {
		vertices.clear();
		float xoffset1 = xoffset + textureWidth;
		float yoffset1 = yoffset + textureHeight;
		float pointPosition[] = {
			x, y, z, xoffset, yoffset,
			x, y - height, z, xoffset, yoffset1,
			x + width, y, z, xoffset1, yoffset,
			x + width, y - height, z, xoffset1, yoffset1
		};
		int indices[] = {
			0, 1, 2,
			2, 1, 3
		};
		for (int i = 0;i < 6;i++) {
			for (int j = 0;j < 5; j++) {
				vertices.push_back(pointPosition[indices[i] * 5 + j]);
			}
		}
	}
	~UI() {
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
		glDeleteTextures(1, &texture);
	}
};