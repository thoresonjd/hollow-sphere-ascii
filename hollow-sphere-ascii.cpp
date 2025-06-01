/**
 * @file hollow-sphere-ascii.cpp
 * @brief A rotating, hollow sphere that renders as ASCII characters in the terminal.
 * @author Justin Thoreson
 * @date 31 May 2025
 */

#include <iostream>
#include <array>
#include <algorithm>
#include <cmath>

namespace {

constexpr int SCREEN_WIDTH = 150;
constexpr int SCREEN_HEIGHT = 50;
constexpr int SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;
constexpr int SCREEN_DISTANCE = 35; // x and y coordinates will be projected onto a screen with distance z' away from the viewer
constexpr float X_OFFSET = SCREEN_WIDTH / 2.0f;
constexpr float Y_OFFSET = SCREEN_HEIGHT / 2.0f;
constexpr float Z_OFFSET = 20.0f;
constexpr float RADIUS = 10.0f;
constexpr float PI = 3.14159265358979323846f;
constexpr float DEGREE_180 = 180.0f;
constexpr float PITCH_DELTA = 0.005f;
constexpr float YAW_DELTA = 0.005f;
constexpr float ROLL_DELTA = 0.001f;
float pitch = 0.0f, yaw = 0.0f, roll = 0.0f;
std::array<char, SCREEN_SIZE> frameBuffer;
std::array<float, SCREEN_SIZE> depthBuffer; // z-buffer

struct Vertex {
	float x, y, z;
};

void clearScreen() noexcept {
	std::cout << "\x1b[2J\x1b[32m";
}

void resetCursor() noexcept {
	std::cout << "\x1b[H";
}

void clearBuffers() noexcept {
	std::fill(frameBuffer.begin(), frameBuffer.end(), ' ');
	std::fill(depthBuffer.begin(), depthBuffer.end(), 0.0f);
}

constexpr float degreesToRadians(const float& angle) noexcept {
	return (angle / DEGREE_180) * PI;
}

float rotateCalcX(const Vertex& vertex, const float& alpha, const float& beta, const float& gamma) noexcept {
	return vertex.x * cos(gamma) * cos(beta) 
		+ vertex.y * (cos(gamma) * sin(beta) * sin(alpha) - sin(gamma) * cos(alpha)) 
		+ vertex.z * (sin(gamma) * sin(alpha) + cos(gamma) * sin(beta) * cos(alpha));	
}

float rotateCalcY(const Vertex& vertex, const float& alpha, const float& beta, const float& gamma) noexcept {
	return vertex.x * sin(gamma) * cos(beta)
		+ vertex.y * (cos(gamma) * cos(alpha) + sin(gamma) * sin(beta) * sin(alpha)) 
		+ vertex.z * (sin(gamma) * sin(beta) * cos(alpha) - cos(gamma) * sin(alpha));
}

float rotateCalcZ(const Vertex& vertex, const float& alpha, const float& beta, [[maybe_unused]] const float& gamma) noexcept {
	return -vertex.x * sin(beta) 
		+ vertex.y * cos(beta) * sin(alpha) 
		+ vertex.z * cos(beta) * cos(alpha);
}

Vertex rotate(const Vertex& vertex, const float& alpha, const float& beta, const float& gamma) noexcept {
	float x = rotateCalcX(vertex, alpha, beta, gamma);
	float y = rotateCalcY(vertex, alpha, beta, gamma);
	float z = rotateCalcZ(vertex, alpha, beta, gamma);
	return {x, y, z};
}

void writeVertex(const Vertex& vertex, const char& character) noexcept {
	const float Z = vertex.z + Z_OFFSET;
	const float OOZ = 1 / Z;
	int xProjection = static_cast<int>(SCREEN_DISTANCE * OOZ * vertex.x * 2);
	int yProjection = static_cast<int>(SCREEN_DISTANCE * OOZ * -vertex.y); // positive y axis goes downward, so y is inverted here
	xProjection += static_cast<int>(X_OFFSET);
	yProjection += static_cast<int>(Y_OFFSET);
	int idx = xProjection+ yProjection * SCREEN_WIDTH;
	if (idx >= 0 && idx < SCREEN_SIZE && OOZ > depthBuffer[idx]) { // depth test
        depthBuffer[idx] = OOZ;
    	frameBuffer[idx] = character;
	}
}

void renderFramebuffer() noexcept {
	for (int i = 0; i < SCREEN_SIZE; i++)
		std::cout << (i % SCREEN_WIDTH ? frameBuffer[i] : '\n');	
}

void initRing(const float& yawOffset, const char& character) noexcept {
	for (float x = -RADIUS; x <= RADIUS; x += 0.025f) {
		float y = sqrt(RADIUS * RADIUS - x * x);
		// top half of ring; sqrt only returns positive
		Vertex vertex = rotate({x, y, 0.0f}, 0.0f, yawOffset, 0.0f);
		vertex = rotate(vertex, pitch, yaw, roll);
		writeVertex(vertex, character);
		// bottom half of ring; account for missing negative from sqrt
		vertex = rotate({x, -y, 0.0f}, 0.0f, yawOffset, 0.0f);
		vertex = rotate(vertex, pitch, yaw, roll);
		writeVertex(vertex, character);
	}
}

void initSphere() noexcept {
	initRing(0.0f, '@');
    initRing(degreesToRadians(45.0f), '$');
	initRing(degreesToRadians(90.0f), '*');
	initRing(degreesToRadians(135.0f), '!');
}

void updateAngles() noexcept {
	pitch += PITCH_DELTA;
	yaw += YAW_DELTA;
	roll += ROLL_DELTA;
}

void render() noexcept {
	clearScreen();
	while (true) {
		clearBuffers();
		resetCursor();
		initSphere();
		renderFramebuffer();
		updateAngles();
	}
}

} // anonymous

int main(void) {
	render();
	return 0;
}

