#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Camera {
public:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::mat4 view;
	float pitch = 0.0f;
	float yaw = -90.0f;
	Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up) {
		cameraPos = pos;
		cameraFront = front;
		cameraUp = up;
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
	void move_forward(float speed) {
		cameraPos += speed * cameraFront;
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
	void move_backward(float speed) {
		cameraPos -= speed * cameraFront;
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
	void move_right(float speed) {
		cameraPos += speed * glm::normalize(glm::cross(cameraFront, cameraUp));
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
	void move_left(float speed) {
		cameraPos -= speed * glm::normalize(glm::cross(cameraFront, cameraUp));
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
	void change_viewport(float xoffset, float yoffset, float sensitivity) {
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		yaw += xoffset;
		pitch += yoffset;
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
	void update() {
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
};
