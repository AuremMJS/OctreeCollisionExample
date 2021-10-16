#define _CRT_SECURE_NO_WARNINGS
#ifndef WINDOW_H
#define WINDOW_H
#include "Constants.h"
#include "Device.h"
#include "UI_Design.h"
#include "Ball.h"


class Window
{
private:
	// Instance to GLFW Window
	GLFWwindow* window;

	// Current translation and last translation values
	GLfloat translate_x, translate_y;
	GLfloat last_x, last_y;

	// Current rotation and last rotation values
	GLfloat rotate_x, rotate_y;
	GLfloat last_rotate_x, last_rotate_y;

	// Current Width and Heigth
	GLfloat width = WIDTH, height = HEIGHT;

	// Bool to check whether mouse is currently in drag event
	bool isMouseDragged;

	// variable to specify which button is clicked now
	int whichButton;

	// arcball for storing light rotation
	BallData lightBall;

	// arcball for storing object rotation
	BallData objectBall;

public:

	Window();
	~Window();

	// Function to initialize the window
	void initWindow();

	GLFWwindow *GetGLFWWindow();

	// Function to handle window resize
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	static void MouseClicked(GLFWwindow* window, int button, int action, int mods);

	static void CursorMoved(GLFWwindow* window, double x, double y);

	static void KeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);

	// Function callback for mouse click
	void mouse_callback(GLFWwindow* window, int button, int action, int mods);

	// Function callback for mouse position change
	void cursor_position_callback(GLFWwindow* window, double x, double y);

	// Function callback for keyboard keys
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	glm::mat4 GetRotationMatrix();
	Matrix4 GetLightRotationMatrix();
	glm::mat4 GetModelMatrix(float scaleValue);
};

#endif