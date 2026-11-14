#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <SFML/Audio.hpp>

std::string TexSides[] = {
	"4_4_4_4_0_1",//grass_block
	"1_1_1_1_1_1",//dirt
	"2_2_2_2_2_2",//stone
	"3_3_3_3_3_3", //bedrock
	"5_5_5_5_5_5",//glass
	"6_6_6_6_6_6",//smooth_stone
	"7_7_7_7_7_7",//diamond_block
	"8_8_8_8_8_8",//gold_block
	"9_9_9_9_9_9"//oak_prank
};

std::string block_sound[]{
	"audio\\blockSounds\\block_grass.ogg",
	"audio\\blockSounds\\block_grass.ogg",
	"audio\\blockSounds\\block_stone.ogg",
	"audio\\blockSounds\\block_stone.ogg",
	"audio\\blockSounds\\block_stone.ogg",
	"audio\\blockSounds\\block_stone.ogg",
	"audio\\blockSounds\\block_stone.ogg",
	"audio\\blockSounds\\block_stone.ogg",
	"audio\\blockSounds\\block_stone.ogg"
};

sf::Music blocksound;

struct BlockType {
	unsigned char* data;
	unsigned int texture;
	unsigned int program;
};

class Block {
public:
	glm::vec3 position;
	float vertices[180];
	float len;

	int nowTexID;
	Block() {
	}
	Block(float xposition, float yposition, float zposition, float length, glm::mat4 Projection, glm::mat4 Model = glm::mat4(1.0), glm::mat4 View = glm::mat4(1.0), int blockID = 0) {
		//Info Setting
		position.x = xposition;
		position.y = yposition;
		position.z = zposition;
		len = length;
		nowTexID = blockID;
	}
	void set_block(float xposition, float yposition, float zposition, float length, unsigned int program, glm::mat4 Projection, glm::mat4 Model = glm::mat4(1.0), glm::mat4 View = glm::mat4(1.0), int blockID = 0) {
		//Info Setting
		position.x = xposition;
		position.y = yposition;
		position.z = zposition;
		len = length;
		nowTexID = blockID;
		if (nowTexID != -1) {
			//VAO,VBO,EBO Settings
			set_vertices(position.x, position.y, position.z, len, TexSides[nowTexID]);
		}
	}
	void get_vertices(std::vector<float> &arr, int startIndex) {
		set_vertices(position.x, position.y, position.z, len, TexSides[nowTexID]);
		for (int i = 0; i < 180; i++) {
			arr[startIndex + i] = vertices[i];
		}
	}
	void set_vertices(float xposition,float yposition, float zposition, float length, std::string Texture) {
		float x = xposition, y = yposition, z = zposition, l = length;
		glm::vec3 p1 = glm::vec3(x, y, z);
		glm::vec3 p2 = glm::vec3(x, y, z + l);
		glm::vec3 p3 = glm::vec3(x + l, y, z + l);
		glm::vec3 p4 = glm::vec3(x + l, y, z);
		glm::vec3 p5 = glm::vec3(x, y - l, z);
		glm::vec3 p6 = glm::vec3(x, y - l, z + l);
		glm::vec3 p7 = glm::vec3(x + l, y - l, z + l);
		glm::vec3 p8 = glm::vec3(x + l, y - l, z);

		int TexID[6], index = 1;
		int s = 0, mul = 1;
		for (int i = Texture.size() - 1;i >= 0; i--) {
			if (Texture[i] == '_') {
				TexID[6 - index] = s;
				s = 0;
				mul = 1;
				index++;

				continue;
			}
			int num = (int)Texture[i] - (int)'0';

			num *= mul;
			mul *= 10;
			s += num;
		}
		TexID[6 - index] = s;
/*
		for (int i = 0;i < 6;i++) {
			printf("%d ", TexID[i]);
		}
		printf("\n");*/

		float xoffset0, yoffset0, xoffset1, yoffset1;
		xoffset0 = (TexID[0] % 8) * 0.125f;
		yoffset0 = std::floor(TexID[0] / 8.0f) * 0.125f;
		xoffset1 = xoffset0 + 0.125f;
		yoffset1 = yoffset0 + 0.125f;

//		std::cout << "id:" << TexID[0] << "\n";
//		std::cout << "xoffset:" << xoffset0 << " yoffset:" << yoffset0 << std::endl;

		//back
		vertices[0] = p1.x;vertices[1] = p1.y;vertices[2] = p1.z;
		vertices[3] = xoffset1;vertices[4] = yoffset0;
		vertices[5] = p4.x;vertices[6] = p4.y;vertices[7] = p4.z;
		vertices[8] = xoffset0;vertices[9] = yoffset0;
		vertices[10] = p8.x;vertices[11] = p8.y;vertices[12] = p8.z;
		vertices[13] = xoffset0;vertices[14] = yoffset1;
		vertices[15] = p8.x;vertices[16] = p8.y;vertices[17] = p8.z;
		vertices[18] = xoffset0; vertices[19] = yoffset1;
		vertices[20] = p5.x;vertices[21] = p5.y;vertices[22] = p5.z;
		vertices[23] = xoffset1;vertices[24] = yoffset1;
		vertices[25] = p1.x;vertices[26] = p1.y;vertices[27] = p1.z;
		vertices[28] = xoffset1;vertices[29] = yoffset0;

		xoffset0 = (TexID[1] % 8) * 0.125f;
		yoffset0 = std::floor(TexID[1] / 8.0f) * 0.125f;
		xoffset1 = xoffset0 + 0.125f;
		yoffset1 = yoffset0 + 0.125f;


		//front
		vertices[30] = p2.x;vertices[31] = p2.y;vertices[32] = p2.z;
		vertices[33] = xoffset0;vertices[34] = yoffset0;
		vertices[35] = p6.x;vertices[36] = p6.y;vertices[37] = p6.z;
		vertices[38] = xoffset0;vertices[39] = yoffset1;
		vertices[40] = p7.x;vertices[41] = p7.y;vertices[42] = p7.z;
		vertices[43] = xoffset1;vertices[44] = yoffset1;
		vertices[45] = p7.x;vertices[46] = p7.y;vertices[47] = p7.z;
		vertices[48] = xoffset1;vertices[49] = yoffset1;
		vertices[50] = p3.x;vertices[51] = p3.y;vertices[52] = p3.z;
		vertices[53] = xoffset1;vertices[54] = yoffset0;
		vertices[55] = p2.x;vertices[56] = p2.y;vertices[57] = p2.z;
		vertices[58] = xoffset0;vertices[59] = yoffset0;

		xoffset0 = (TexID[2] % 8) * 0.125f;
		yoffset0 = std::floor(TexID[2] / 8.0f) * 0.125f;
		xoffset1 = xoffset0 + 0.125f;
		yoffset1 = yoffset0 + 0.125f;
		//left
		vertices[60] = p6.x;vertices[61] = p6.y;vertices[62] = p6.z;
		vertices[63] = xoffset1;vertices[64] = yoffset1;
		vertices[65] = p2.x;vertices[66] = p2.y;vertices[67] = p2.z;
		vertices[68] = xoffset1;vertices[69] = yoffset0;
		vertices[70] = p1.x;vertices[71] = p1.y;vertices[72] = p1.z;
		vertices[73] = xoffset0;vertices[74] = yoffset0;
		vertices[75] = p1.x;vertices[76] = p1.y;vertices[77] = p1.z;
		vertices[78] = xoffset0;vertices[79] = yoffset0;
		vertices[80] = p5.x;vertices[81] = p5.y;vertices[82] = p5.z;
		vertices[83] = xoffset0;vertices[84] = yoffset1;
		vertices[85] = p6.x;vertices[86] = p6.y;vertices[87] = p6.z;
		vertices[88] = xoffset1;vertices[89] = yoffset1;

		xoffset0 = (TexID[3] % 8) * 0.125f;
		yoffset0 = std::floor(TexID[3] / 8.0f) * 0.125f;
		xoffset1 = xoffset0 + 0.125f;
		yoffset1 = yoffset0 + 0.125f;
		//right
		vertices[90] = p7.x;vertices[91] = p7.y;vertices[92] = p7.z;
		vertices[93] = xoffset0;vertices[94] = yoffset1;
		vertices[95] = p8.x;vertices[96] = p8.y;vertices[97] = p8.z;
		vertices[98] = xoffset1;vertices[99] = yoffset1;
		vertices[100] = p4.x;vertices[101] = p4.y;vertices[102] = p4.z;
		vertices[103] = xoffset1;vertices[104] = yoffset0;
		vertices[105] = p4.x;vertices[106] = p4.y;vertices[107] = p4.z;
		vertices[108] = xoffset1;vertices[109] = yoffset0;
		vertices[110] = p3.x;vertices[111] = p3.y;vertices[112] = p3.z;
		vertices[113] = xoffset0;vertices[114] = yoffset0;
		vertices[115] = p7.x;vertices[116] = p7.y;vertices[117] = p7.z;
		vertices[118] = xoffset0;vertices[119] = yoffset1;

		xoffset0 = (TexID[4] % 8) * 0.125f;
		yoffset0 = std::floor(TexID[4] / 8.0f) * 0.125f;
		xoffset1 = xoffset0 + 0.125f;
		yoffset1 = yoffset0 + 0.125f;
		//up
		vertices[120] = p1.x;vertices[121] = p1.y;vertices[122] = p1.z;
		vertices[123] = xoffset0;vertices[124] = yoffset0;
		vertices[125] = p2.x;vertices[126] = p2.y;vertices[127] = p2.z;
		vertices[128] = xoffset0;vertices[129] = yoffset1;
		vertices[130] = p3.x;vertices[131] = p3.y;vertices[132] = p3.z;
		vertices[133] = xoffset1;vertices[134] = yoffset1;
		vertices[135] = p3.x;vertices[136] = p3.y;vertices[137] = p3.z;
		vertices[138] = xoffset1;vertices[139] = yoffset1;
		vertices[140] = p4.x;vertices[141] = p4.y;vertices[142] = p4.z;
		vertices[143] = xoffset1;vertices[144] = yoffset0;
		vertices[145] = p1.x;vertices[146] = p1.y;vertices[147] = p1.z;
		vertices[148] = xoffset0;vertices[149] = yoffset0;

		xoffset0 = (TexID[5] % 8) * 0.125f;
		yoffset0 = std::floor(TexID[5] / 8.0f) * 0.125f;
		xoffset1 = xoffset0 + 0.125f;
		yoffset1 = yoffset0 + 0.125f;
		//down
		vertices[150] = p5.x;vertices[151] = p5.y;vertices[152] = p5.z;
		vertices[153] = xoffset0;vertices[154] = yoffset1;
		vertices[155] = p8.x;vertices[156] = p8.y;vertices[157] = p8.z;
		vertices[158] = xoffset1;vertices[159] = yoffset1;
		vertices[160] = p7.x;vertices[161] = p7.y;vertices[162] = p7.z;
		vertices[163] = xoffset1;vertices[164] = yoffset0;
		vertices[165] = p7.x;vertices[166] = p7.y;vertices[167] = p7.z;
		vertices[168] = xoffset1;vertices[169] = yoffset0;
		vertices[170] = p6.x;vertices[171] = p6.y;vertices[172] = p6.z;
		vertices[173] = xoffset0;vertices[174] = yoffset0;
		vertices[175] = p5.x;vertices[176] = p5.y;vertices[177] = p5.z;
		vertices[178] = xoffset0;vertices[179] = yoffset1;
	}
};
class Chunk {
public:
	std::vector<Block> blocks;
	std::vector<float> vertices;
	std::vector<Block> glassVector;
	std::vector<float> glassVertices;
	int x, y;
	unsigned int VAO, VBO, GLASSVAO, GLASSVBO;
	unsigned int ProgramShader;
	unsigned int texture;
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;
	std::vector<float> r_vertices;
	int bid;
/*	void operator=(Chunk c) {
		blocks = c.blocks;
		vertices = c.vertices;
		x = c.x;
		y = c.y;
		projection = c.projection;
		view = c.view;
		model = c.model;
	}*/
	Chunk() {
		bid = 0;
	}
/*	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;

	Chunk(Chunk&& other) noexcept : VAO(other.VAO), ProgramShader(other.ProgramShader), VBO(other.VBO) {
		other.VAO = 0;
		other.ProgramShader = 0;
		other.VBO = 0;
	}
	Chunk& operator=(Chunk&& other) noexcept {
		if (this != &other) {
			if (VAO != 0) glDeleteVertexArrays(1, &VAO); // 清理自身旧资源
			VAO = other.VAO;
			other.VAO = 0; // 转移所有权
			if (ProgramShader != 0) glDeleteProgram(ProgramShader); // 清理自身旧资源
			ProgramShader = other.ProgramShader;
			other.ProgramShader = 0; // 转移所有权
			if (VBO != 0) glDeleteProgram(VBO); // 清理自身旧资源
			VBO = other.VBO;
			other.VBO = 0; // 转移所有权
		}
		return *this;
	}*/

	void Generate(int cx, int cy, glm::mat4 Projection, glm::mat4 View, glm::mat4 Model, bool restart = false) {
		x = cx; y = cy;
		projection = Projection;
		view = View;
		model = Model;
		if (restart) {
			vertices.clear();
			blocks.clear();
		}
		blocks.resize(15360);
		glassVector.resize(15360);
		vertices.resize(2764800);
		glassVertices.resize(2764800);

		for (int k = 0;k < 60;k++) {
			for (int j = 0; j < 16;j++) {
				for (int i = 0;i < 16; i++) {
//					printf("%d\n", k * 256 + j * 16 + i);
					if (k > 20) {
						blocks[k * 256 + j * 16 + i].set_block(x * 16 * 0.6 + i * 0.6, k * 0.6, y * 16 * 0.6 + j * 0.6, 0.6f, ProgramShader, projection, model, view, -1);
						for (int l = 0;l < 180;l++) {
							vertices[(k * 256 + j * 16 + i) * 180 + l] = 0.0f;
						}
					}
					else if (k == 20) {
						blocks[k * 256 + j * 16 + i].set_block(x * 16 * 0.6 + i * 0.6, k * 0.6, y * 16 * 0.6 + j * 0.6,  0.6f,ProgramShader, projection, model, view, 0);
						blocks[k * 256 + j * 16 + i].get_vertices(vertices, (k * 256 + j * 16 + i) * 180);
					}
					else if (k < 20 && k > 16) {
						blocks[k * 256 + j * 16 + i].set_block(x * 16 * 0.6 + i * 0.6, k * 0.6,y * 16 * 0.6 + j * 0.6,  0.6f ,ProgramShader, projection, model, view, 1);
						blocks[k * 256 + j * 16 + i].get_vertices(vertices, (k * 256 + j * 16 + i) * 180);
					}
					else if (k <= 16 && k > 2) {
						blocks[k * 256 + j * 16 + i].set_block(x * 16 * 0.6 + i * 0.6, k * 0.6, y * 16 * 0.6 + j * 0.6, 0.6f, ProgramShader, projection, model, view, 2);
						blocks[k * 256 + j * 16 + i].get_vertices(vertices, (k * 256 + j * 16 + i) * 180);
					}else{
						blocks[k * 256 + j * 16 + i].set_block(x * 16 * 0.6 + i * 0.6,k * 0.6, y * 16 * 0.6 + j * 0.6,  0.6f, ProgramShader, projection, model, view, 3);
						blocks[k * 256 + j * 16 + i].get_vertices(vertices, (k * 256 + j * 16 + i) * 180);
					}
				}
			}
		}
		std::string fsShaderSource = "#version 330 core\n"
			"in vec2 TexCoord;\n"
			"out vec4 FragColor;\n"
			"uniform sampler2D ourTexture;\n"
			"void main()\n"
			"{\n"
			"    FragColor = texture(ourTexture, TexCoord);\n"
			"}";
		std::string vsShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec2 aTexCoord;\n"
			"out vec2 TexCoord;\n"
			"uniform mat4 model;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"   TexCoord = aTexCoord;\n"
			"}";
		const char* vertexShaderSource = vsShaderSource.c_str();
		const char* fragmentShaderSource = fsShaderSource.c_str();
		int  success;
		char infoLog[1024];

		//			std::cout << vertexShaderSource << "\n\n\n" << fragmentShaderSource << "\n";
		GLint vLen = strlen(vertexShaderSource);
		GLint fLen = strlen(fragmentShaderSource);

		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, &vLen);
		std::cout << "编译着色器(0/2)...\n";
		glCompileShader(vertexShader);
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, &fLen);
		std::cout << "编译着色器(1/2)...\n";
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		ProgramShader = glCreateProgram();
		glAttachShader(ProgramShader, vertexShader);
		glAttachShader(ProgramShader, fragmentShader);
		glLinkProgram(ProgramShader);
		glGetProgramiv(ProgramShader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ProgramShader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		std::vector<float> r_vertices;
		int flag = 0;
		for (auto i = 0; i < vertices.size(); i++) {
			if (i % 5 == 0) {
				if (vertices[i] == 0.0f && vertices[i + 1] == 0.0f && vertices[i + 2] == 0.0f && vertices[i + 3] == 0.0f && vertices[i + 4] == 0.0f) {
					flag = 5;
					flag--;
					continue;
				}
			}
			if (flag > 0) {
				flag--;
				continue;
			}
			r_vertices.push_back(vertices[i]);
		}
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		std::cout << VAO << std::endl;
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindVertexArray(VAO);
		glBufferData(GL_ARRAY_BUFFER, r_vertices.size() * sizeof(float), r_vertices.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &GLASSVBO);
		glGenVertexArrays(1, &GLASSVAO);
		glBindBuffer(GL_ARRAY_BUFFER, GLASSVBO);
		glBindVertexArray(GLASSVAO);
		glBufferData(GL_ARRAY_BUFFER, glassVertices.size() * sizeof(float), glassVertices.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		r_vertices.clear();
		r_vertices.shrink_to_fit();

		int width, height, nrChannel;
		unsigned char* data = stbi_load("texture\\blocks_texture.png", &width, &height, &nrChannel, 4);
		if (!data) {
			std::cout << "WARNING::WARNING::TEXTURE WAS NOT LOADED::WARNING::WARNING\n";
			const char* reason = stbi_failure_reason();
			std::cout << "Error Reason:" << reason << "\n";
		}
//		printf("%d\n",nrChannel);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);

		int counter = 0;
/*		for (auto i = 0; i < vertices.size(); i++) {
//			std::cout << vertices[i] << " ";
//			if ((i + 1) % 5 == 0) std::cout << std::endl;
			if (vertices[i] == 0.125f && (i + 1) % 5 == 4) {
				std::cout << "Error!\n";
				counter += 1;
			}
		}
		std::cout << counter << std::endl;*/
	}
	void chunk_Update() {
/*		for (int k = 0;k < 60;k++) {
			for (int j = 0; j < 16;j++) {
				for (int i = 0;i < 16; i++) {
					int id = k * 256 + j * 16 + i;
					if (blocks[id].nowTexID == -1) {
						for (int l = 0;l < 180;l++) {
							vertices[(k * 256 + j * 16 + i) * 180 + l] = 0.0f;
						}
					}
					else {
						blocks[id].get_vertices(vertices, (k * 256 + j * 16 + i) * 180);
					}
				}
			}
		}*/
		r_vertices.clear();
		int flag = 0;
		for (auto i = 0; i < vertices.size(); i++) {
			if (i % 5 == 0) {
				if (vertices[i] == 0.0f && vertices[i + 1] == 0.0f && vertices[i + 2] == 0.0f && vertices[i + 3] == 0.0f && vertices[i + 4] == 0.0f) {
					flag = 5;
					flag--;
					continue;
				}
			}
			if (flag > 0) {
				flag--;
				continue;
			}
			r_vertices.push_back(vertices[i]);
		}
//		std::cout << r_vertices.size() << std::endl;

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindVertexArray(VAO);
		glBufferData(GL_ARRAY_BUFFER, r_vertices.size() * sizeof(float), r_vertices.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, GLASSVBO);
		glBindVertexArray(GLASSVAO);
		glBufferData(GL_ARRAY_BUFFER, glassVertices.size() * sizeof(float), glassVertices.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

	}
	void face_removal() {
		glassVector.clear();
		glassVertices.clear();

		for (int k = 0;k < 60;k++) {
			for (int j = 0; j < 16;j++) {
				for (int i = 0;i < 16; i++) {
					int id = k * 256 + j * 16 + i;
					if (blocks[id].nowTexID == -1) {
						for (int l = 0;l < 180;l++) {
							vertices[(k * 256 + j * 16 + i) * 180 + l] = 0.0f;
						}
					}
					else if (blocks[id].nowTexID == 4) {
						glassVector.push_back(blocks[id]);
						for (int l = 0;l < 180;l++) {
							vertices[(k * 256 + j * 16 + i) * 180 + l] = 0.0f;
						}
						blocks[id].set_vertices(blocks[id].position.x, blocks[id].position.y, blocks[id].position.z, blocks[id].len, TexSides[blocks[id].nowTexID]);
						for (int i = 0; i < 180; i++) {
							glassVertices.push_back(blocks[id].vertices[i]);
						}
					}
					else {

						blocks[id].set_vertices(blocks[id].position.x, blocks[id].position.y, blocks[id].position.z, blocks[id].len, TexSides[blocks[id].nowTexID]);
//						printf("%d %d\n", id, bid);
						for (int i = 0; i < 180; i++) {
							vertices[id * 180 + i] = blocks[id].vertices[i];
						}
					}
				}
			}
		}
		for (int i = 0; i < 16; i++) {
			for (int j = 0;j < 16;j++) {
				for (int k = 0;k < 60;k++) {
					int blockID = k * 256 + j * 16 + i;
					if (blocks[blockID].nowTexID == -1 || blocks[blockID].nowTexID == 4)continue;
					if (i - 1 < 0 || i + 1 >= 16) {

					}
					else {
						if (blocks[blockID - 1].nowTexID != -1) {
							if (blocks[blockID - 1].nowTexID != 4) {
							for (int l = 60;l < 90;l++) {
								vertices[blockID * 180 + l] = 0.0f;
							}
							}

						}
						if (blocks[blockID + 1].nowTexID != -1) {
							if (blocks[blockID + 1].nowTexID != 4) {
							for (int l = 90;l < 120;l++) {
								vertices[blockID * 180 + l] = 0.0f;
							}
							}

						}
					}
					if (j - 1 < 0 || j + 1 >= 16) {

					}
					else {

						if (blocks[blockID - 16].nowTexID != -1) {
							if (blocks[blockID - 16].nowTexID != 4) {
							for (int l = 0;l < 30;l++) {
								vertices[blockID * 180 + l] = 0.0f;
							}
							}

						}
						if (blocks[blockID + 16].nowTexID != -1) {
							if (blocks[blockID + 16].nowTexID != 4) {
							for (int l = 30;l < 60;l++) {
								vertices[blockID * 180 + l] = 0.0f;
							}
							}

						}
					}
					if (k - 1 < 0 || k + 1 >= 60) {

					}
					else {
						if (blocks[blockID - 256].nowTexID != 4) {
							if (blocks[blockID - 256].nowTexID != 4) {
							for (int l = 150;l <180;l++) {
								vertices[blockID * 180 + l] = 0.0f;
							}
							}

						}
						if (blocks[blockID + 256].nowTexID != -1) {
							if (blocks[blockID + 256].nowTexID != 4) {
							for (int l = 120;l < 150;l++) {
								vertices[blockID * 180 + l] = 0.0f;
							}
							}

						}
					}
				}
			}
		}
		chunk_Update();
	}
	void draw() {
		glUseProgram(ProgramShader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		int modelLoc = glGetUniformLocation(ProgramShader, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(ProgramShader, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projectionLoc = glGetUniformLocation(ProgramShader, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, r_vertices.size() / 5);
		glBindVertexArray(0);
		
//		glDisable(GL_CULL_FACE);
//		glDepthMask(GL_FALSE);
//		glBindVertexArray(GLASSVAO);
//		glDrawArrays(GL_TRIANGLES, 0, glassVertices.size() / 5);
//		glBindVertexArray(0);
//		glDepthMask(GL_TRUE);
//		glEnable(GL_CULL_FACE);
	}
	void update_view(glm::mat4 View) {
		view = View;
	}
	void clear() {
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &GLASSVBO);
		glDeleteVertexArrays(1, &GLASSVAO);
		glDeleteProgram(ProgramShader);
	}
	int checkBlock(float x, float y, float z) {
		for (auto i = 0;i < blocks.size();i++) {
			glm::vec3 pos = blocks[i].position;
			float len = blocks[i].len;
			if (x > pos.x && x < pos.x + len) {
				if (y > pos.y && y < pos.y + len) {
					if (z > pos.z && z < pos.z + len && blocks[i].nowTexID != -1) {
						return i;
					}
				}
			}
		}
		return -1;
	}
	void breakBlock(int id) {
		if (!blocksound.openFromFile(block_sound[blocks[id].nowTexID])) {
			std::cout << "Error loading blocksound." << std::endl;
			std::cerr << "Error loading blocksound." << std::endl;
		}
		else {
			blocksound.setVolume(100);
			blocksound.play();
		}
		blocks[id].nowTexID = -1;
		face_removal();
	}
	void addBlock(int id, int Texid) {
		if (!blocksound.openFromFile(block_sound[Texid])) {
			std::cout << "Error loading blocksound." << std::endl;
			std::cerr << "Error loading blocksound." << std::endl;
		}
		else {
			blocksound.setVolume(100);
			blocksound.play();
		}
		blocks[id].nowTexID = Texid;
		bid = id;
		face_removal();
	}
	~Chunk() {
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &GLASSVBO);
		if(VAO != 0){
			glDeleteVertexArrays(1, &VAO);
			glDeleteVertexArrays(1, &GLASSVAO);
		}
		if(ProgramShader != 0)
			glDeleteProgram(ProgramShader);
		r_vertices.clear();
		r_vertices.shrink_to_fit();
	}
};