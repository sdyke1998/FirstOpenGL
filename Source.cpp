#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shader.h>
#include <camera.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Model.h>
#include <Light.h>

float deltaTime;
float pitch;
glm::vec3 camPos;
glm::vec3 camFront;
glm::vec3 worldUp;
Camera theCamera;
Camera_Movement currentDirection;

const int w = 1000;
const int h = 1000;
const float PI = 3.1416f;
const float lightStrength = 0.85f;
const glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
const glm::mat4 I4(1.0f);

//Lighting attenuation constants
const float constant = 1.0f;
const float linear = 0.14f;
const float quadratic = 0.07f;

bool firstMouse = true;
bool spotLightOn = false;
float ratio = 0.5f;
float yaw = -90.0f;
float lastX = ((float)w) / 2.0f;
float lastY = ((float)h) / 2.0f;
glm::vec3 lightPos(0.0f, 0.0f, 10.0f);

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void right_click_callback(GLFWwindow* window, int button, int action, int mods);

void printVec3(glm::vec3 vect);
void printFloat(float num);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

	glViewport(0, 0, width, height);
}

int main() {

	theCamera.MouseXSensitivity = 1.0f;
	theCamera.MouseYSensitivity = 1.0f;
	theCamera.MovementSpeed = 2.5f;

	//GLFW & GLAD setup
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(w, h, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}
	
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, right_click_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {

		std::cout << "Failed to initialise GLAD" << std::endl;
		return 1;
	}

	//Camera setup
	camPos = glm::vec3(0.0f, 0.0f, 3.0f);
	camFront = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	theCamera.WorldUp = worldUp;
	theCamera.Front = camFront;

	//Graphics setup
	Shader ourShader("shaders/vertexSrc.vs", "shaders/fragmentSrc.fs");
	Shader lightSourceShader("shaders/lightVS.vs", "shaders/lightFS.fs");
	Shader pointLightShader("shaders/lightVS.vs", "shaders/lightFS.fs");

	DirLight sun(lightPos, camFront);
	PointLight blueCube(glm::vec3(0.0f), 20.0f);
	SpotLight flashLight(theCamera.Position, glm::radians(15.0f), glm::radians(5.0f), 10.0f);

	/*
	blueCube.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	blueCube.setColour();
	blueCube.setAmbient();
	blueCube.setDiffuse();
	blueCube.setSpecular();
	blueCube.setBrightness();
	*/

	Shader sunShader("shaders/lightVS.vs", "shaders/lightFS.fs");
	Shader blueCubeShader("shaders/lightVS.vs", "shaders/lightFS.fs");

	sun.initShader(sunShader);

	int texWidth, texHeight, nrChannels, texW1, texH1, nrChr1;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("brickSpec.jpg", &texWidth, &texHeight, &nrChannels, 0);
	unsigned char *data1 = stbi_load("brickwall.jpg", &texW1, &texH1, &nrChr1, 0);

	const unsigned int VA_LEN = 8;
	float vertices[] = {
	//     Vertex(3)      Tex(2)         Normals(3)
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

	-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f
	};

	//vertices2[] is an array containing only the vertices in vertices[], without texture coordinates or normals
	float vertices2[108];

	for (int i = 1; i <= 6; i++) {
		for (int j = 1; j <= 6; j++) {
			for (int k = 1; k <= 3; k++) {

				int index = i * j * k - 1;
				vertices2[index] = vertices[index];
			}
		}
	}

	//Creating translation matrix for container1
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
	
	unsigned int tex, tex1;
	unsigned int VBOs[3];
	unsigned int VAOs[3];

	glGenTextures(1, &tex);
	glGenTextures(1, &tex1);
	glGenBuffers(3, VBOs);
	glGenVertexArrays(3, VAOs);

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
			GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VA_LEN * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VA_LEN * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VA_LEN * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Setup for sun
	glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Setup for point light
	glBindVertexArray(VAOs[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 model = glm::rotate(
		glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)
	);
	//View and projection matrices are calculated every frame

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	//Finding the centre and radius of circle of point light's path
	glm::vec3 centre(0.0f);
	float lightRadius = 0.0f;
	for (int i = 0; i < 10; i++) {

		centre += cubePositions[i];

		float len = glm::length(cubePositions[i]);
		if (len > lightRadius) lightRadius = len;
	}
	centre /= 10.0f;
	glm::vec3 pointLightPos = centre + glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 pointLightCol(0.0f, 0.0f, 1.0f);

	float glfwTime; //Used for time dependent transformations in shader(s)
	const float radius = 10.0f;

	deltaTime = 0.0f;
	float lastFrame = 0.0f;

	//Render loop
	while (!glfwWindowShouldClose(window)) {

		glfwTime = glfwGetTime();
		deltaTime = glfwTime - lastFrame;
		lastFrame = glfwTime;

		glEnable(GL_DEPTH_TEST);
		processInput(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		//Camera setup
		view = theCamera.GetViewMatrix();
	
		//Camera fov
		proj = glm::perspective(
			glm::radians(theCamera.Zoom), ((float)w) / ((float)h), 0.1f, 100.0f
		);

		//Drawing light cube
		//glBindVertexArray(VAOs[1]);
		model = glm::translate(I4, lightPos);

		sun.useShader();
		sun.setModelMat(model);
		sun.setViewMat(view);
		sun.setProjMat(proj);
		sun.setColour(lightColor);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Drawing point light cube
		//glBindVertexArray(VAOs[2]);

		//2πtf, where f = 5/60 = 1/12. Full rotation made every 5 secs.
		float circleArgs = 2.0f * PI * glfwTime/(5.0f * 60.0f);

		glm::vec3 circlePath =
			lightRadius * glm::vec3(glm::cos(circleArgs), 0.0f, glm::sin(circleArgs));

		model = glm::translate(I4, pointLightPos + circlePath);

		pointLightShader.use();
		pointLightShader.setVec3("lightColor", pointLightCol);
		pointLightShader.setMat4("model", model);
		pointLightShader.setMat4("view", view);
		pointLightShader.setMat4("proj", proj);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Drawing container1
		glBindVertexArray(VAOs[0]);
		ourShader.use();
		glUniform1i(glGetUniformLocation(ourShader.ID, "material.Specular"), 0);
		ourShader.setInt("brickTex", 1);
		ourShader.setVec3("sun.direction", 0.0f, 0.0f, -1.0f);
		ourShader.setVec3("sun.ambient", 0.2f, 0.2f, 0.2f);
		ourShader.setVec3("sun.diffuse", glm::vec3(lightStrength));
		ourShader.setVec3("sun.specular", 1.0f, 1.0f, 1.0f);

		ourShader.setFloat("pointLight.constant", constant);
		ourShader.setFloat("pointLight.linear", linear);
		ourShader.setFloat("pointLight.quadratic", quadratic);
		ourShader.setVec3("pointLight.position", pointLightPos + circlePath);
		ourShader.setVec3("pointLight.ambient", 0.2f * pointLightCol);
		ourShader.setVec3("pointLight.diffuse", pointLightCol);
		ourShader.setVec3("pointLight.specular", pointLightCol);

		ourShader.setFloat("spotLight.constant", constant);
		ourShader.setFloat("spotLight.linear", linear);
		ourShader.setFloat("spotLight.quadratic", quadratic);
		ourShader.setVec3("spotLight.position", theCamera.Position);
		ourShader.setFloat("spotLight.cosPhi", glm::cos(glm::radians(5.0f)));
		ourShader.setFloat("spotLight.cosGamma", glm::cos(glm::radians(7.5f)));
		ourShader.setVec3("spotLight.spotDir", theCamera.Front);
		ourShader.setVec3("spotLight.ambient", glm::vec3(0.2f));
		ourShader.setVec3("spotLight.diffuse", glm::vec3(1.0f));
		ourShader.setVec3("spotLight.specular", glm::vec3(1.0f));

		ourShader.setVec3("viewPos", theCamera.Position);
		ourShader.setVec3("material.Ambient", glm::vec3(1.0f));
		ourShader.setVec3("material.Diffuse", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("material.shininess", 128.0f);

		glUniform1ui(glGetUniformLocation(ourShader.ID, "toggleSpotLight"), spotLightOn);
		
		//Adding 3D coordinate transformations
		unsigned int model_loc = glGetUniformLocation(ourShader.ID, "model");
		unsigned int view_loc = glGetUniformLocation(ourShader.ID, "view");
		unsigned int proj_loc = glGetUniformLocation(ourShader.ID, "proj");

		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));
		
		//Texture loading
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex1);
		
		//Rendering all cubes
		for (unsigned int i = 0; i < 10; i++) {
			float angle = 20.0f * ((float)i);

			model = glm::translate(
				glm::mat4(1.0f), cubePositions[i]
			);

			model = glm::rotate(
				model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)
			);

			if (i % 3 == 0) {
				glm::vec4 rotVec = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
				rotVec = glm::normalize(model * rotVec);

				angle = 2.0f * glfwTime * PI;
				model = glm::rotate(
					model, glm::radians(angle), glm::vec3(rotVec.x, rotVec.y, rotVec.z)
				);
			}
	
			ourShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//Events and buffer swap
		glfwSwapBuffers(window);
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		currentDirection = FORWARD;
		theCamera.ProcessKeyboard(currentDirection, deltaTime);
	}
		
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		currentDirection = BACKWARD;
		theCamera.ProcessKeyboard(currentDirection, deltaTime);
	}
		
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		currentDirection = LEFT;
		theCamera.ProcessKeyboard(currentDirection, deltaTime);
	}
		
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		currentDirection = RIGHT;
		theCamera.ProcessKeyboard(currentDirection, deltaTime);
	}
	else {
		theCamera.ProcessKeyboard(currentDirection, 0.0f);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	theCamera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	
	theCamera.ProcessMouseScroll(yoffset);
}

void right_click_callback(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {

		spotLightOn = !spotLightOn;
	}
}

void printVec3(glm::vec3 vect) {
	std::cout << "(" << vect.x << ", " << vect.y << ", " << vect.z << ")" << std::endl;
}
void printFloat(float num) {
	std::cout << num << std::endl;
}