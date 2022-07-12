#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader.h>

class Light {

public:
	Light(glm::vec3 pos) {

		this->position = pos;
	}
	Light() {
		this->pShader = 0;
		delete pShader;
	}

	glm::vec3 getAmbient();
	glm::vec3 getDiffuse();
	glm::vec3 getSpecular();
	glm::vec3 getColour();
	glm::vec3 getPosition();
	float getBrightness();

	void setAmbient(glm::vec3 am);
	void setDiffuse(glm::vec3 diff);
	void setSpecular(glm::vec3 spec);
	void setColour(glm::vec3 col);
	void setPosition(glm::vec3 pos);
	void setBrightness(float bri);

	void setModelMat(glm::mat4 model);
	void setViewMat(glm::mat4 view);
	void setProjMat(glm::mat4 proj);

	void initShader(Shader s);

	void useShader();

protected:
	glm::vec3 colour;
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float brightness = 1.0f;

	Shader* pShader = 0;
};

class DirLight : public Light {

public:
	DirLight(glm::vec3 pos, glm::vec3 dir) {

		this->position = pos;
		this->direction = dir;
	}
	DirLight() {}

	glm::vec3 getDirection();

	void setDirection(glm::vec3 dir);

private:
	glm::vec3 direction;
};

class PointLight : public Light {

public:
	PointLight(glm::vec3 pos, float range) {

		this->position = pos;
		SetupAttenuation(range);
	}
	PointLight() {}

	glm::vec3 getConstants();

	void setConstants(float con, float lin, float quad);
	void setConstants(glm::vec3 cons);

private:
	float constant = 1.0f;
	float linear;
	float quadratic;

	void SetupAttenuation(float range) {

		this->linear =
			4.5f * glm::pow(range, -1.0f) - 2.0f * glm::pow(range, -2.0f) + 34.5f * glm::pow(range, -3.0f);
		this->quadratic =
			0.6f * glm::pow(range, -1.0f) + 49.4f * glm::pow(range, -2.0f) + 242.4f * glm::pow(range, -3.0f);
	}
};

class SpotLight : public Light {

public:
	SpotLight(glm::vec3 pos, float cos_out, float cos_in, float range) {

		this->position = pos;
		this->cos_outer = cos_out;
		this->cos_inner = cos_in;
		SetupAttenuation(range);
	}
	SpotLight() {}

	glm::vec3 getConstants();
	glm::vec3 getDirection();
	glm::vec2 getAngles();
	float getInnerAngle();
	float getOuterAngle();

	void setConstants(float con, float lin, float quad);
	void setConstants(glm::vec3 cons);
	void setDirection(glm::vec3 dir);
	void setInnerAngle(float cos_in);
	void setOuterAngle(float cos_out);
	void setAngles(glm::vec2 cos_angs);

private:
	float constant = 1.0f;
	float linear;
	float quadratic;

	glm::vec3 direction;

	float cos_outer;
	float cos_inner;

	void SetupAttenuation(float range) {

		this->linear =
			4.5f * glm::pow(range, -1.0f) - 2.0f * glm::pow(range, -2.0f) + 34.5f * glm::pow(range, -3.0f);
		this->quadratic =
			0.6f * glm::pow(range, -1.0f) + 49.4f * glm::pow(range, -2.0f) + 242.4f * glm::pow(range, -3.0f);
	}
};

//Light getters
glm::vec3 Light::getAmbient() {
	return this->ambient;
}
glm::vec3 Light::getDiffuse() {
	return this->diffuse;
}
glm::vec3 Light::getSpecular() {
	return this->specular;
}
glm::vec3 Light::getColour() {
	return this->colour;
}
glm::vec3 Light::getPosition() {
	return this->position;
}
float Light::getBrightness() {
	return this->brightness;
}
//Light setters
void Light::setAmbient(glm::vec3 am) {
	this->ambient = am;
}
void Light::setDiffuse(glm::vec3 diff) {
	this->diffuse = diff;
}
void Light::setSpecular(glm::vec3 spec) {
	this->specular = spec;
}
void Light::setColour(glm::vec3 col) {

	pShader->setVec3("lightColor", col);
	this->colour = col;
}
void Light::setPosition(glm::vec3 pos) {

	this->position = pos;
}
void Light::setBrightness(float bri) {
	this->brightness = bri;
}

void Light::setModelMat(glm::mat4 model) {

	pShader->setMat4("model", model);
}
void Light::setViewMat(glm::mat4 view) {

	pShader->setMat4("view", view);
}
void Light::setProjMat(glm::mat4 proj) {

	pShader->setMat4("proj", proj);
}
void Light::initShader(Shader s) {

	pShader = &s;
}
void Light::useShader() {

	pShader->use();
}

//DirLight getters
glm::vec3 DirLight::getDirection() {
	return this->direction;
}
//DirLight setters
void DirLight::setDirection(glm::vec3 dir) {

	this->direction = dir;
}

//PointLight getters
glm::vec3 PointLight::getConstants() {

	return glm::vec3(this->constant, this->linear, this->quadratic);
}
//PointLight setters
void PointLight::setConstants(float con, float lin, float quad) {

	this->constant = con;
	this->linear = lin;
	this->quadratic = quad;
}
void PointLight::setConstants(glm::vec3 cons) {

	this->constant = cons[0];
	this->linear = cons[1];
	this->quadratic = cons[2];
}

//SpotLight getters
glm::vec3 SpotLight::getConstants() {

	return glm::vec3(this->constant, this->linear, this->quadratic);
}
glm::vec3 SpotLight::getDirection(){
	return this->direction;
}
glm::vec2 SpotLight::getAngles() {

	return glm::vec2(this->cos_inner, this->cos_outer);
}
float SpotLight::getInnerAngle() {
	return this->cos_inner;
}
float SpotLight::getOuterAngle() {
	return this->cos_outer;
}
//SpotLight setters
void SpotLight::setConstants(float con, float lin, float quad) {

	this->constant = con;
	this->linear = lin;
	this->quadratic = quad;
}
void SpotLight::setConstants(glm::vec3 cons) {

	this->constant = cons[0];
	this->linear = cons[1];
	this->quadratic = cons[2];
}
void SpotLight::setDirection(glm::vec3 dir) {
	this->direction = dir;
}
void SpotLight::setInnerAngle(float cos_in) {
	this->cos_inner = cos_in;
}
void SpotLight::setOuterAngle(float cos_out) {
	this->cos_outer = cos_out;
}
void SpotLight::setAngles(glm::vec2 cos_angs) {
	this->cos_inner = cos_angs[0];
	this->cos_outer = cos_angs[1];
}

#endif