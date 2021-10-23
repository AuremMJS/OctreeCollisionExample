#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h> // Include GLFW to render the objects in a window
#include "Window.h"
#include "Application.h"
#include "UI_Design.h"


void Window::MouseClicked(GLFWwindow * glfwWindow, int button, int action, int mods)
{
	auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	window->mouse_callback(glfwWindow, button, action, mods);
}

void Window::CursorMoved(GLFWwindow* glfwWindow, double x, double y)
{
	auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	window->cursor_position_callback(glfwWindow, x, y);
}

void Window::KeyPressed(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
	auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	window->key_callback(glfwWindow, key, scancode, action, mods);
}

Window::Window()
{
	// initialise arcballs to 80% of the widget's size
	Ball_Init(&lightBall);		Ball_Place(&lightBall, qOne, 0.80);
	Ball_Init(&objectBall);		Ball_Place(&objectBall, qOne, 0.80);
	isMouseDragged = false;
	whichButton = -1;
}

Window::~Window()
{
}

// Function to initializes a Window to render the objects
void Window::initWindow() {

	// Set initial translate and rotation
	translate_x = translate_y = 0.0;
	rotate_x = rotate_y = 0.0;

	// Initialize the GLFW Library
	glfwInit();

	// Specify GLFW not to create an OpenGL Context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// Restrict Window resizing
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	// Create a Window
	// 1st Parameter - Width of the Window
	// 2nd Parameter - Height of the Window
	// 3rd Parameter - Title of the Window
	// 4th Parameter - Monitor to open the window
	// 5th Parameter - Only relevant to OpenGL
	window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_TITLE, nullptr, nullptr);
	// Set the pointer to window in callback functions
	glfwSetWindowUserPointer(window, this);

	// Initialize the callback function for window resize
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

	// Initialize the callback function for mouse events
	glfwSetMouseButtonCallback(window, MouseClicked);

	glfwSetCursorPosCallback(window, CursorMoved);

	// Initialize the callback function for keyboard events
	glfwSetKeyCallback(window, KeyPressed);

}

GLFWwindow * Window::GetGLFWWindow()
{
	return window;
}

// Callback function called when window is resized
void Window::framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height) {
	// Get the pointer to the application
	auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(glfwWindow));
	app->OnWindowResized();

	auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	window->width = width;
	window->height = height;
}

// Callback function called when mouse position changes
void Window::cursor_position_callback(GLFWwindow* window, double x, double y) {
	HVect vNow;
	// scale both coordinates from that
	float size = (width > height) ? height : width;

	if (isMouseDragged)
	{
		if (whichButton == GLFW_MOUSE_BUTTON_RIGHT)
		{
			vNow.x = (2.0 * x - size) / size;
			vNow.y = (size - 2.0 * y) / size;

			// pass it to the arcball code	
			Ball_Mouse(&objectBall, vNow);
			// start dragging
			Ball_Update(&objectBall);
		}
		if (whichButton == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			vNow.x = 0.0;
			vNow.y = (size - 2.0 * y) / size;

			// pass it to the arcball code	
			Ball_Mouse(&lightBall, vNow);
			// start dragging
			Ball_Update(&lightBall);
		}
		if (whichButton == GLFW_MOUSE_BUTTON_LEFT) {
			vNow.x = (2.0 * x - size) / size;
			vNow.y = (size - 2.0 * y) / size;
			translate_x = vNow.x - last_x;
			translate_y = vNow.y - last_y;
			last_x = vNow.x;
			last_y = vNow.y;
		}
	}
}

// Mouse callback function
void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;
	double x;
	double y;
	glfwGetCursorPos(window, &x, &y);
	Vec4 vNow;
	HVect vNow2;
	float size = (width > height) ? height : width;
	vNow.x = (2.0 * x - size) / size;
	vNow.y = (size - 2.0 * y) / size;
	// scale both coordinates from that

	if (button == GLFW_MOUSE_BUTTON_LEFT) {

		if (GLFW_PRESS == action)
		{
			last_x = vNow.x;
			last_y = vNow.y;
			isMouseDragged = true;
			whichButton = GLFW_MOUSE_BUTTON_LEFT;
		}
		else if (GLFW_RELEASE == action)
		{
			translate_x = vNow.x - last_x;
			translate_y = vNow.y - last_y;
			last_x = vNow.x;
			last_y = vNow.y;
			isMouseDragged = false;
			whichButton = -1;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		vNow2.x = 0.0;
		vNow2.y = (size - 2.0 * y) / size;
		if (GLFW_PRESS == action)
		{
			last_rotate_x = vNow.x;
			last_rotate_y = vNow.y;
			// pass it to the arcball code	
			Ball_Mouse(&lightBall, vNow2);
			// start dragging
			Ball_BeginDrag(&lightBall);
			isMouseDragged = true;
			whichButton = GLFW_MOUSE_BUTTON_MIDDLE;
		}
		else if (GLFW_RELEASE == action)
		{
			rotate_x += vNow.x - last_rotate_x;
			rotate_y += vNow.y - last_rotate_y;
			last_rotate_x = vNow.x;
			last_rotate_y = vNow.y;
			// end the drag
			Ball_EndDrag(&lightBall);
			isMouseDragged = false;
			whichButton = -1;
		}

	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		vNow2.x = (2.0 * x - size) / size;
		vNow2.y = (size - 2.0 * y) / size;
		if (GLFW_PRESS == action)
		{
			last_rotate_x = vNow.x;
			last_rotate_y = vNow.y;
			// pass it to the arcball code	
			Ball_Mouse(&objectBall, vNow2);
			// start dragging
			Ball_BeginDrag(&objectBall);
			isMouseDragged = true;
			whichButton = GLFW_MOUSE_BUTTON_RIGHT;
		}
		else if (GLFW_RELEASE == action)
		{
			rotate_x += vNow.x - last_rotate_x;
			rotate_y += vNow.y - last_rotate_y;
			last_rotate_x = vNow.x;
			last_rotate_y = vNow.y;
			// end the drag
			Ball_EndDrag(&objectBall);
			isMouseDragged = false;
			whichButton = -1;
		}
	}
}

// Keyboard callback functions
void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
}

glm::mat4 Window::GetRotationMatrix()
{
	float mNow[16];
	Ball_Value(&objectBall, mNow);
	return glm::make_mat4(mNow);
}

Matrix4 Window::GetLightRotationMatrix()
{
	float mNow[16];
	Ball_Value(&lightBall, mNow);
	return Matrix4(mNow);
}

glm::mat4 Window::GetModelMatrix(float scaleValue)
{
	glm::mat4 rot = GetRotationMatrix();
	glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(scaleValue));
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f + translate_x * 2, translate_y * 2, 0.0f));
	glm::mat4 model = scale * translate *rot;

	return model;
}

glm::vec3 Window::GetTranslateValues()
{
	return glm::vec3(translate_x,translate_y,0.0f);
}

bool Window::ShouldUpdate()
{
	return isMouseDragged;
}

