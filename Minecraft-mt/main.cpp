#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "block.h"
#include <cstdio>
#include <Windows.h>
#include <Psapi.h>
#include <stdint.h>
#include <SFML/Audio.hpp>
#include <thread>
#include <chrono>
#include <fstream>
#include "ui.h"
#include <ft2build.h>
#include <map>
#include "text.h"
#include <cmath>
#include FT_FREETYPE_H

#define SIZE SIZES

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;
float SCR_R = 0.0f;
float SCR_G = 1.0f;
float SCR_B = 1.0f;
int polygonMode = 0;
float delta_time;
float last_frame;
bool firstMouse = true;
float lastX = 400, lastY = 300;
float sensitivity = 0.1f;
int texid = 0;
int bigMapx;
int bigMapy;

Camera camera(glm::vec3(5.0f, 15.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

const char* vertexShaderSource1 = "#version 330 core\n"
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
const char* fragmentShaderSource1 = "#version 330 core\n"
"in vec2 TexCoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"    FragColor = texture(ourTexture, TexCoord);\n"
"}";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
std::vector<Chunk> map;
std::vector<Chunk> bigMap;
bool showMem = true;
bool showBlock = true;
float vx, vy;
glm::vec3 lastPos = camera.cameraPos;
int SIZE;
UI switcher;
int fast_inventory[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9
};
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		polygonMode = 1 - polygonMode;
		if (!polygonMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
	}
	bool switch_block = false;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		texid = 0;
		switch_block = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		texid = 1;
		switch_block = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		texid = 2;
		switch_block = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		texid = 3;
		switch_block = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
		texid = 4;
		switch_block = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
		texid = 5;
		switch_block = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
		texid = 6;
		switch_block = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
		texid = 7;
		switch_block = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
		texid = 8;
		switch_block = 1;
	}
	if (switch_block) {
		switcher.setVertices(-0.65f + texid * 0.143f, -0.7f, 0.0f, 0.155f, 0.2f, 0.0f, 0.085f, 0.09375f, 0.09375f);
	}
	float forwardSpeed = 0.0005f;
	float sensitivity = 0.97f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		vy += forwardSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		vy -= forwardSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		vx -= forwardSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		vx += forwardSpeed;
	}
	vy *= sensitivity;
	vx *= sensitivity;
	float pointChunkx = std::floor(camera.cameraPos.x / 16.0 / 0.6) + SIZE / 2;
	float pointChunky = std::floor(camera.cameraPos.z / 16.0 / 0.6) + SIZE / 2;
	if (!((pointChunkx * SIZE + pointChunky) < 0 || (pointChunkx * SIZE + pointChunky) > map.size())) {
		int result = map[pointChunkx * SIZE + pointChunky].checkBlock(camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z);
		if (result != -1) {
			camera.move_forward(vy);
			camera.move_right(vx);
		}
		else {
			camera.move_forward(vy);
			camera.move_right(vx);
		}
	}
	lastPos = camera.cameraPos;

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		std::string command;
		std::cout << "请输入指令 >";
		std::cin >> command;
		while (command != "close") {
			if (command == "setMemoryShows-false") {
				std::cout << "已将setMemoryShows更新为false" << std::endl;
				showMem = false;
			}
			else if (command == "setMemoryShows-true") {
				std::cout << "已将setMemoryShows更新为true" << std::endl;
				showMem = true;
			}
			else if (command == "setBlockUpdateShows-false") {
				std::cout << "已将setBlockUpdateShows更新为false" << std::endl;
				showBlock = false;
			}
			else if (command == "setBlockUpdateShows-true") {
				std::cout << "已将setBlockUpdateShows更新为true" << std::endl;
				showBlock = true;
			}
			else if (command == "keys") {
				std::cout << "wasd移动..." << std::endl;
			}
			else if (command == "shutdown") {
				std::cout << "游戏关闭..." << std::endl;
				glfwTerminate();
			}
			else if (command == "help") {
				std::cout << "指令如下：\n--------------\nhelp:查询指令\nclose返回游戏\nkeys显示按键提示\nsetMemoryShows-bools:设置是否输出内存提示，bools替换成true为显示，false为隐藏.\nsetBlockUpdateShows-bools:设置是否在放方块是显示信息，bools为true为是，false为否\nshutdown:强制关闭游戏\n---------------\n";
			}
			else {
				std::cout << "指令疑似输入错误，可以输入help指令查询所有指令,close可以返回游戏。" << std::endl;
			}
			std::cout << "请输入指令 >";
			std::cin >> command;
		}
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
	lastX = xpos;
	lastY = ypos;
	camera.change_viewport(xoffset, yoffset, sensitivity);
}

std::vector<float> glassVector;
unsigned int GLASSVAO, GLASSVBO;
int right, left, up, down;

void mouse_button_call_back(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glm::vec3 eyePosition = camera.cameraPos + glm::vec3(0.0, 0.5, 0.0);
		glm::vec3 pointPosition;
		for (float i = 0.0;i < 2.0;i += 0.01) {
			pointPosition = eyePosition + camera.cameraFront * i;
			float pointChunkx = std::floor(pointPosition.x / 16.0 / 0.6) + SIZE / 2;
			float pointChunky = std::floor(pointPosition.z / 16.0 / 0.6) + SIZE / 2;
			if (!((pointChunkx * SIZE + pointChunky) < 0 || (pointChunkx * SIZE + pointChunky) > map.size())) {
				//				std::cout << pointChunkx << " " << pointChunky << std::endl;
				int result = map[pointChunkx * SIZE + pointChunky].checkBlock(pointPosition.x, pointPosition.y, pointPosition.z);
				if (result != -1) {
					std::cout << "Located Chunk:" << pointChunkx << ", " << pointChunky << " BlockId:" << result << "\n";
					map[pointChunkx * SIZE + pointChunky].breakBlock(result);

					glassVector.clear();
					for (int i = 0; i < map.size();i++) {
						glassVector.insert(glassVector.end(), map[i].glassVertices.begin(), map[i].glassVertices.end());
					}

					glBindBuffer(GL_ARRAY_BUFFER, GLASSVBO);
					glBindVertexArray(GLASSVAO);
					glBufferData(GL_ARRAY_BUFFER, glassVector.size() * sizeof(float), glassVector.data(), GL_DYNAMIC_DRAW);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
					glEnableVertexAttribArray(1);
					
					break;
				}
			}
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		glm::vec3 eyePosition = camera.cameraPos + glm::vec3(0.0, 0.5, 0.0);
		glm::vec3 pointPosition;
		glm::vec3 last_result = eyePosition;
		for (float i = 0.0;i < 2.0;i += 0.01) {
			pointPosition = eyePosition + camera.cameraFront * i;
			float pointChunkx = std::floor(pointPosition.x / 16.0 / 0.6) + SIZE / 2;
			float pointChunky = std::floor(pointPosition.z / 16.0 / 0.6) + SIZE / 2;
			if (!((pointChunkx * SIZE + pointChunky) < 0 || (pointChunkx * SIZE + pointChunky) > map.size())) {
				int result = map[pointChunkx * SIZE + pointChunky].checkBlock(pointPosition.x, pointPosition.y, pointPosition.z);
				if (result != -1) {
					if (showBlock) {
						std::cout << "Located Chunk:" << pointChunkx << ", " << pointChunky << " BlockId:" << result << "\n";
						std::cout << "BlockID:" << texid << "\n";
					}
					if (last_result.y > pointPosition.y && last_result.y > map[pointChunkx * SIZE + pointChunky].blocks[result].position.y + map[pointChunkx * SIZE + pointChunky].blocks[result].len) {
						if (result + 256 < map[pointChunkx * SIZE + pointChunky].blocks.size()) {
							map[pointChunkx * SIZE + pointChunky].addBlock(result + 256, texid);
						}
						else {
							std::cout << "You can't put blocks over 60 blocks high.\n";
						}
					}
					else if (last_result.y < pointPosition.y && last_result.y < map[pointChunkx * SIZE + pointChunky].blocks[result].position.y) {
						if (result - 256 >= 0) {
							map[pointChunkx * SIZE + pointChunky].addBlock(result - 256, texid);
						}
						else {
							std::cout << "You can't put blocks in the void.\n";
						}
					}
					else if (last_result.x < pointPosition.x && last_result.x < map[pointChunkx * SIZE + pointChunky].blocks[result].position.x) {
						if (result - 1 >= 0 && result % 16 > 0) {
							map[pointChunkx * SIZE + pointChunky].addBlock(result - 1, texid);
						}
						else {
							if (pointChunkx > 0 && pointChunkx < SIZE) {
								std::cout << pointChunkx * SIZE + pointChunky - 1 << "\n";
								map[(pointChunkx - 1) * SIZE + pointChunky].addBlock(result + 15, texid);
							}
							else {
								std::cout << "You can't put blocks out of the world.\n";
							}
						}
					}
					else if (last_result.x > pointPosition.x && last_result.x > map[pointChunkx * SIZE + pointChunky].blocks[result].position.x + map[pointChunkx * SIZE + pointChunky].blocks[result].len) {
						if (result + 1 < map[pointChunkx * SIZE + pointChunky].blocks.size() && result % 16 < 15) {
							map[pointChunkx * SIZE + pointChunky].addBlock(result + 1, texid);
						}
						else {
							if (pointChunkx > 0 && pointChunkx < SIZE) {
								std::cout << pointChunkx * SIZE + pointChunky - 1 << "\n";
								map[(pointChunkx + 1) * SIZE + pointChunky].addBlock(result - 15, texid);
							}
							else {
								std::cout << "You can't put blocks out of the world.\n";
							}
						}
					}
					else if (last_result.z > pointPosition.z && last_result.z > map[pointChunkx * SIZE + pointChunky].blocks[result].position.z + map[pointChunkx * SIZE + pointChunky].blocks[result].len) {
						if (result + 16 < map[pointChunkx * SIZE + pointChunky].blocks.size() && (result + 16) % 256 > 15) {
							map[pointChunkx * SIZE + pointChunky].addBlock(result + 16, texid);
						}
						else {
							if (pointChunky > 0 && pointChunky < SIZE) {
								std::cout << pointChunkx * SIZE + pointChunky - 1 << "\n";
								map[pointChunkx * SIZE + pointChunky + 1].addBlock(result - 15 * 16, texid);
							}
							else {
								std::cout << "You can't put blocks out of the world.\n";
							}
						}
					}
					else if (last_result.z < pointPosition.z && last_result.z < map[pointChunkx * SIZE + pointChunky].blocks[result].position.z) {
						if (result - 1 >= 0 && result % 256 > 15) {
							map[pointChunkx * SIZE + pointChunky].addBlock(result - 16, texid);
						}
						else {
							if (pointChunky > 0 && pointChunky < SIZE) {
								std::cout << pointChunkx * SIZE + pointChunky - 1 << "\n";
								map[pointChunkx * SIZE + pointChunky - 1].addBlock(result + 15 * 16, texid);
							}
							else {
								std::cout << "You can't put blocks out of the world.\n";
							}
						}
					}
					glassVector.clear();
					for (int i = 0; i < map.size();i++) {
						glassVector.insert(glassVector.end(), map[i].glassVertices.begin(), map[i].glassVertices.end());
					}

					glBindBuffer(GL_ARRAY_BUFFER, GLASSVBO);
					glBindVertexArray(GLASSVAO);
					glBufferData(GL_ARRAY_BUFFER, glassVector.size() * sizeof(float), glassVector.data(), GL_DYNAMIC_DRAW);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
					glEnableVertexAttribArray(1);
					break;
					break;
				}
			}
		}
	}
}



int modelLoc, viewLoc, projectionLoc;
int chunkShowNums = 3;
int playerChunkLocx = std::floor(camera.cameraPos.x / 16 / 0.6f);
int playerChunkLocy = std::floor(camera.cameraPos.y / 16 / 0.6f);
int MemCounter = 0;
double FPS;

glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
glm::mat4 model = glm::mat4(1.0);
bool LoadingChunks = false;	
uint64_t ComputerMemory;
MEMORYSTATUSEX memInfo;
/*	if (playerChunkLocx + SIZE / 2 + chunkShowNums > right + std::floor(SIZE / 2)) {
		std::cout << "Out!" << std::endl;
		int bigMapxx = bigMapx;
		bigMapx++;
		right++;
		map.resize((bigMapxx + 1)* bigMapy);
		for (auto i = bigMapxx * bigMapy; i < bigMapxx * bigMapy + bigMapy;i++) {
			int worldX = right - 1;
			int worldY = i - bigMapxx * bigMapy - SIZE / 2;
			map[i].Generate(worldX, worldY, projection, camera.view, model, true);
			map[i].face_removal();
		}

	}*/
bool gameRunning = true;
std::ofstream logfile("log.txt");
std::ifstream options("options.txt");

void Log() {

}
int main() {
	glassVector.reserve(2764800);
	logfile << "Start to initiate map..." << std::endl;
	map.reserve(256 * 256);
	bigMap.reserve(256 * 256);
	up = down = right = left = std::ceil(SIZE / 2.0);
	logfile << "Initiated map." << std::endl;
	logfile << "Initiate glfw and opengl." << std::endl;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	logfile << "Create Window." << std::endl;

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Minecraft", NULL, NULL);
	if (window == NULL) {
		std::cout << "ERROR::ERROR::You loser you can even not open a window hahaha!::ERROR::ERROR" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	logfile << "Load Glad." << std::endl;

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "ERROR::ERROR::Your GLAD has already died.::ERROR::ERROR" << std::endl;
		return -1;
	}

	logfile << "Setting window." << std::endl;
	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_call_back);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Font consola18("fonts\\consola.ttf", 18);
	consola18.loadChar();
	consola18.render();

	glm::mat4 ortho = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);


//	Block b(0.5f, 0.3f, 0.3f, 0.6f, projection, glm::mat4(1.0), camera.view);
	std::cout << "Generating Maps, Please Wait..." << std::endl;
	logfile << "Generating Maps, Please Wait..." << std::endl;

	int MAPSIZE = 0;
	std::string options_reader;
	if (options.is_open()) {
		std::getline(options, options_reader);
		int p = 1;
		for (int i = options_reader.size() - 1;i > options_reader.find('=');i--) {
			MAPSIZE += ((int)options_reader[i] - (int)'0') * p;
			p *= 10;
		}
	}
	else {
		MAPSIZE = 1000;
	}
	SIZE = MAPSIZE;

	UI LS;
	LS.Generate();
	LS.setShader();
	LS.setVertices(-0.5f, 0.2f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	UI LBg;
	LBg.Generate();
	LBg.setShader();
	LBg.setVertices(-1.0f, 1.0f, 0.0f, 2.0f, 2.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	bigMapx = SIZE;
	bigMapy = SIZE;
	std::cout << MAPSIZE << std::endl;
	map.resize(SIZE * SIZE);
//	bigMap.resize(SIZE * SIZE);
	int numOfChunks = 0;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {

			LBg.setTexture("icons\\loading\\isles.png");


			LS.setTexture("icons\\loading\\window.png");

			glClearColor(SCR_R, SCR_G, SCR_B, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			LBg.draw(ortho, model);
			LS.draw(ortho, model);

			consola18.RenderText("Loading Chunks " + std::to_string((float)numOfChunks / SIZE / SIZE * 100.0f) + "%", WIDTH / 2.0F - 120.0f, HEIGHT / 2.0F-50.0f, 1.0F, glm::vec3(1.0f, 1.0f, 1.0f), WIDTH, HEIGHT);
			glfwSwapBuffers(window);
			glfwPollEvents();
			int worldX = i - SIZE / 2;
			int worldY = j - SIZE / 2;
			map[i * SIZE + j].Generate(worldX, worldY, projection, camera.view, model, true);
//			bigMap[i * SIZE + j].Generate(worldX, worldY, projection, camera.view, model, true);
//			std::cout << "Loading...(" << i * SIZE + j << "/" << SIZE * SIZE << ")" << "\n";
			logfile << "Loading...(" << i * SIZE + j << "/" << SIZE * SIZE << ")" << "\n";
			map[i * SIZE + j].face_removal();
			numOfChunks++;
		}
	}

	LBg.setTexture("icons\\loading\\isles.png");


	LS.setTexture("icons\\loading\\window.png");

	glClearColor(SCR_R, SCR_G, SCR_B, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	LBg.draw(ortho, model);
	LS.draw(ortho, model);

	consola18.RenderText("Loading Chunks " + std::to_string((float)numOfChunks / SIZE / SIZE * 100.0f) + "%", WIDTH / 2.0F - 120.0f, HEIGHT / 2.0F - 50.0f, 1.0F, glm::vec3(1.0f, 1.0f, 1.0f), WIDTH, HEIGHT);
	glfwSwapBuffers(window);
	glfwPollEvents();

	glGenBuffers(1, &GLASSVBO);
	glGenVertexArrays(1, &GLASSVAO);
	glassVector.clear();
	for (int i = 0; i < map.size();i++) {
		glassVector.insert(glassVector.end(), map[i].glassVertices.begin(), map[i].glassVertices.end());
	}

	glBindBuffer(GL_ARRAY_BUFFER, GLASSVBO);
	glBindVertexArray(GLASSVAO);
	glBufferData(GL_ARRAY_BUFFER, glassVector.size() * sizeof(float), glassVector.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

//	std::cout << "Loading Music..." << std::endl;
	logfile << "Loading Music..." << std::endl;
	sf::Music BgMusic;
	if (!BgMusic.openFromFile("audio\\minecraft.ogg")) {
		logfile << "Error loading background music." << std::endl;
		std::cerr << "Error loading background music." << std::endl;
	}
	else {
		BgMusic.setLoop(true);
		BgMusic.setVolume(50);
		BgMusic.play();
	}
//	std::cout << "Loading UIs..." << std::endl;
	//物品栏
	std::string uiVShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec2 aTexCoord;\n"
		"out vec2 TexCoord;\n"
		"uniform mat4 model;\n"
//		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main(){\n"
		"    gl_Position = projection * model * vec4(aPos, 1.0);\n"
		"    TexCoord = aTexCoord;\n"
		"}\0";
	std::string uiFShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoord;\n"
		"uniform sampler2D ourTexture;\n"
		"void main(){\n"
		"    FragColor = texture(ourTexture, TexCoord);\n"
		"}\0";
/*
	float itembarVertices[] = {
		 -0.7f, -0.9f,0.0f, 0.0f, 0.085f,
		 -0.7f,-0.7f, 0.0f, 0.0f, 0.0f,
		 0.7f,-0.7f, -0.0f, 0.71f, 0.0f,
		 0.7f,-0.7f, -0.0f, 0.71f, 0.0f,
		 0.7f, -0.9f,-0.0f, 0.71f, 0.085f,
		 -0.7f, -0.9f,-0.0f, 0.0f, 0.085f
	};*/

	UI itembar;
	itembar.Generate();
	itembar.setShader();
	itembar.setVertices(-0.65f, -0.7f, 0.0f, 1.3f, 0.2f, 0.0f, 0.0f, 0.71f, 0.085f);
	itembar.setTexture("icons\\bag\\widgets.png");


	switcher.Generate();
	switcher.setShader();
	switcher.setVertices(-0.65f, -0.7f, 0.0f, 0.155f, 0.2f, 0.0f, 0.085f, 0.09375f, 0.09375f);
	switcher.setTexture("icons\\bag\\widgets.png");

	UI fast_inventory_ui;
	fast_inventory_ui.Generate();
	fast_inventory_ui.setShader();
	fast_inventory_ui.setVertices(-0.6265f, -0.722f, 0.0f, 0.11f, 0.155f, 0.0f, 0.0f, 1.0f, 1.0f);

	std::string fiui[] = {
		"0.png",
		"1.png",
		"2.png",
		"3.png",
		"4.png",
		"5.png",
		"6.png",
		"7.png",
		"8.png"
	};
	std::string fiuiRoad = "icons\\items\\";

	std::string realfiuiRoad = fiuiRoad + fiui[texid];
	fast_inventory_ui.setTexture(realfiuiRoad);
//	std::cout << "Done." << std::endl;

	std::thread login(Log);
	login.detach();
	
	glm::mat4 itembarModel = glm::mat4(1.0f);
	glm::mat4 switcherModel = glm::mat4(1.0f);

	LBg.setTexture("icons\\loading\\isles.png");


	LS.setTexture("icons\\loading\\window.png");

	glClearColor(SCR_R, SCR_G, SCR_B, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	LBg.draw(ortho, model);
	LS.draw(ortho, model);

	consola18.RenderText("Done.", WIDTH / 2.0F - 20.0f, HEIGHT / 2.0, 1.0F, glm::vec3(1.0f, 1.0f, 1.0f), WIDTH, HEIGHT);
	glfwSwapBuffers(window);
	glfwPollEvents();
	Sleep(1000);

	while (!glfwWindowShouldClose(window)){
		processInput(window);

//		itembarModel = glm::translate(itembarModel, camera.cameraPos);
		playerChunkLocx = std::floor(camera.cameraPos.x / 16 / 0.6f);
		playerChunkLocy = std::floor(camera.cameraPos.z / 16 / 0.6f);

//		printf("%d %d\n", playerChunkLocx + left, playerChunkLocy + down);

		glClearColor(SCR_R, SCR_G, SCR_B, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float current_time = glfwGetTime();
		delta_time = current_time - last_frame;
		last_frame = current_time;
		camera.update();



//		std::cout << camera.cameraPos.x << std::endl;
/*		if (playerChunkLocx + SIZE / 2 + chunkShowNums > right + std::floor(SIZE / 2)) {
			std::cout << "加载区块..." << std::endl;
			map.resize((bigMapx + 1) * bigMapy);
			for (auto i = bigMapx * bigMapy; i < bigMapx * bigMapy + bigMapy;i++) {
				int worldX = right;
				int worldY = i - bigMapx * bigMapy - SIZE / 2;
				map[i].Generate(worldX, worldY, projection, camera.view, model, true);
				map[i].face_removal();
				std::cout << "worldX:" << worldX << " worldY:" << worldY << std::endl;
			}
			right++;
			bigMapx++;
			for (int i = 0; i < bigMapx; i++) {
				for (int j = 0;j < bigMapy;j++) {
					std::cout << map[i * bigMapy + j].VAO << " ";
				}
				std::cout << std::endl;
			}
		}
		if (playerChunkLocx + SIZE / 2 - chunkShowNums < -left + std::floor(SIZE / 2)) {
			std::cout << "加载区块..." << std::endl;
			for (auto i = 0; i < bigMapy;i++) {
				int worldX = -left;
				int worldY = i - SIZE / 2;
				map.insert(map.begin() + i, Chunk());
				map[i].Generate(worldX, worldY, projection, camera.view, model, true);
				map[i].face_removal();
				std::cout << "worldX:" << worldX << " worldY:" << worldY << std::endl;
			}
			left++;
			bigMapx++;
			std::cout << bigMapx << " " << bigMapy << std::endl;
			for (int i = 0; i < bigMapx; i++) {
				for (int j = 0;j < bigMapy;j++) {
					std::cout << map[i * bigMapy + j].VAO << " ";
				}
				std::cout << std::endl;
			}
		}
//		printf("%d %d\n", playerChunkLocy + chunkShowNums, up);
		if (playerChunkLocy + chunkShowNums > up) {
			std::cout << "加载区块..." << std::endl;
			for (auto i = 0; i < bigMapx;i++) {
				std::cout << "index:" << i * bigMapy + i << " size:" << bigMap.size() << std::endl;
				int worldX = map[i * bigMapy + i + 1].x;
				int worldY = up;

				map.insert(map.begin() + i * bigMapy + i, Chunk());
				map[i * bigMapy + i].Generate(worldX, worldY, projection, camera.view, model, true);
				map[i * bigMapy + i].face_removal();
				std::cout << "worldX:" << worldX << " worldY:" << worldY << std::endl;
			}
			up++;
			bigMapy++;
			for (int i = 0; i < bigMapx; i++) {
				for (int j = 0;j < bigMapy;j++) {
					std::cout << map[i * bigMapy + j].VAO << " ";
				}
				std::cout << std::endl;
			}
		}
		if (playerChunkLocy - chunkShowNums < -down) {
			std::cout << "加载区块..." << std::endl;
			for (auto i = 0; i < bigMapx;i++) {
				std::cout << "index:" << i * bigMapy + bigMapy << " size:" << map.size() << std::endl;
				int worldX = map[i * bigMapy + bigMapy + i - 1].x;
				int worldY = -down;

				map.insert(map.begin() + i * bigMapy + bigMapy + i, Chunk());
				map[i * bigMapy + bigMapy + i].Generate(worldX, worldY, projection, camera.view, model, true);
				map[i * bigMapy + bigMapy + i].face_removal();
				std::cout << "worldX:" << worldX << " worldY:" << worldY << std::endl;
			}
			down++;
			bigMapy++;
			for (int i = 0; i < bigMapx; i++) {
				for (int j = 0;j < bigMapy;j++) {
					std::cout << map[i * bigMapy + j].VAO << " ";
				}
				std::cout << std::endl;
			}
		}*/
//		printf("max:%d %d %d %d\n", (bigMapx-1)*bigMapy + bigMapy - 1, map.size(), bigMapx, bigMapy);
//		std::cout << "len:" << map.size() << " BX:" << bigMapx << " BY:" << bigMapy << std::endl;
		glDisable(GL_BLEND);
		for (int i = 0; i < bigMapx; i++) {
			for (int j = 0;j < bigMapy;j++) {
//				printf("%d\n", bigMapx / 2);
//				if(std::abs(playerChunkLocx - (i - bigMapx / 2)) < chunkShowNums && std::abs(playerChunkLocy - (j - bigMapy / 2)) < chunkShowNums){
//					std::cout << "MapIndex:" << i * bigMapy + j << std::endl;
					map[i * bigMapy + j].update_view(camera.view);
					map[i * bigMapy + j].draw();
//				}
			}
		}
		glEnable(GL_BLEND);


		glUseProgram(map[0].ProgramShader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map[0].texture);
		int modelLoc = glGetUniformLocation(map[0].ProgramShader, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(map[0].ProgramShader, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(map[0].view));
		int projectionLoc = glGetUniformLocation(map[0].ProgramShader, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(map[0].projection));
		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);
		glBindVertexArray(GLASSVAO);
		glDrawArrays(GL_TRIANGLES, 0, glassVector.size() / 5);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);

		itembar.draw(ortho, itembarModel);
		for (int i = 0;i < 9;i++) {
			realfiuiRoad = fiuiRoad + fiui[fast_inventory[i]];
			fast_inventory_ui.setTexture(realfiuiRoad);
			fast_inventory_ui.setVertices(-0.6265f + i * 0.143f, -0.722f, 0.0f, 0.11f, 0.155f, 0.0f, 0.0f, 1.0f, 1.0f);
			fast_inventory_ui.draw(ortho, model);
		}
		fast_inventory_ui.draw(ortho, model);
		switcher.draw(ortho, switcherModel);


		FPS = 1.0 / delta_time;
		if (std::floor(FPS) < 1)FPS = 1;
//		printf("FPS: %f \n", FPS);
		consola18.RenderText("FPS: "+std::to_string(FPS), 10.0f, HEIGHT - 20.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), WIDTH, HEIGHT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	gameRunning = false;
	for (int i = 0; i < bigMapx; i++) {
		for (int j = 0;j < bigMapy;j++) {
			map[i * SIZE + j].clear();
		}
	}
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
//			bigMap[i * SIZE + j].clear();
		}
	}
	glfwTerminate();
	logfile.close();
	return 0;
}