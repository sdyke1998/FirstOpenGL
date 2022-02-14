#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shader.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const int w = 1000;
const int h = 1000;
float ratio = 0.5f;

void processInput(GLFWwindow* window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

	glViewport(0, 0, width, height);
}

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(w, h, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {

		std::cout << "Failed to initialise GLAD" << std::endl;
		return 1;
	}

	Shader ourShader("shaders/vertexSrc.vs", "shaders/fragmentSrc.fs");
	Shader ourShader2("shaders/vertexSrc2.vs", "shaders/fragmentSrc.fs");

	int texWidth, texHeight, nrChannels, texW1, texH1, nrChr1;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("awesomeface.png", &texWidth, &texHeight, &nrChannels, 0);
	unsigned char* data1 = stbi_load("brickwall.jpg", &texW1, &texH1, &nrChr1, 0);

	//Rectangle vertices1 and colours
	float vertices1[] = {
	//Vertex(2)     Colour(3)      Tex(2)
	-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, //Top left
	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, //Bottom left
	0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, //Bottom Right
	0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f //Top right
	};
	unsigned int indices[] = {

		0, 1, 3,
		1, 2, 3
	};
	
	//Creating translation matrix for container1
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
	
	unsigned int tex, tex1;
	unsigned int VBOs[2];
	unsigned int VAOs[2];
	unsigned int EBOs[2];

	glGenTextures(1, &tex);
	glGenTextures(1, &tex1);
	glGenBuffers(2, EBOs);
	glGenBuffers(2, VBOs);
	glGenVertexArrays(2, VAOs);

	glBindVertexArray(VAOs[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Loading both texture images
	if (data) {

		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else std::cout << "Failed to load texture :(" << std::endl;
	stbi_image_free(data);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data1) {

		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB, texW1, texH1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else std::cout << "Failed to load texture :(" << std::endl;
	stbi_image_free(data1);

	//Setting up vertex attributes for container1
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) (5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Setup for container2 (displayed at top left of screen)
	glBindVertexArray(VAOs[1]);

	

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glm::mat4 trans2 = glm::mat4(1.0f);
	//trans2 = glm::translate(trans2, glm::vec3(0.0f, 0.0f, 0.0f));
	unsigned int trans2_loc = glGetUniformLocation(ourShader2.ID, "transMat");
	glUniformMatrix4fv(trans2_loc, 1, GL_FALSE, glm::value_ptr(trans2));

	float glfwTime; //Used for time dependent transformations in shader(s)

	//Render loop
	while (!glfwWindowShouldClose(window)) {

		processInput(window);

		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Drawing container1
		//Changing texture mix for container1
		ourShader.use();
		ourShader.setFloat("ratio", ratio);
		glUniform1i(glGetUniformLocation(ourShader.ID, "ourTex"), 0);
		ourShader.setInt("nexTex", 1);
		
		//Rotating container1 over time
		glfwTime = (float)glfwGetTime();
		trans = glm::rotate(trans, glm::radians(glfwTime*6/60), glm::vec3(0.0f, 0.0f, 1.0f));
		unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transMat");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex1);

		glBindVertexArray(VAOs[0]);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Drawing container2
		ourShader2.use();
		ourShader2.setFloat("time", glfwTime);
		ourShader2.setFloat("ratio", ratio);
		ourShader2.setInt("ourTex", 0);
		ourShader2.setInt("nexTex", 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex1);

		glBindVertexArray(VAOs[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Events and buffer swap
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(2, VAOs);
	glDeleteBuffers(2, VBOs);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) ratio += 0.025f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) ratio -= 0.025f;
}