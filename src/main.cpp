#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <atomic>

#ifndef _GLIBCXX_USE_NANOSLEEP
#define _GLIBCXX_USE_NANOSLEEP
#endif
#include <thread>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLSL.h"
#include "Program.h"
#include "Camera.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "Scene.h"

using namespace std;
using namespace Eigen;

bool keyToggles[256] = {false}; // only for English keyboards!

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from

int track_number = 0; // Track number to load

shared_ptr<Camera> camera;
shared_ptr<Program> prog;
shared_ptr<Program> progSimple;
shared_ptr<Scene> scene;

// https://stackoverflow.com/questions/41470942/stop-infinite-loop-in-different-thread
std::atomic<bool> stop_flag;

static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		stop_flag = true;
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	else if (key == GLFW_KEY_UP || key == 'W') {
		scene->moveForward();
	}

	else if (key == GLFW_KEY_DOWN || key == 'S') {
		scene->moveBackward();
	}
}

static void char_callback(GLFWwindow *window, unsigned int key)
{
	keyToggles[key] = !keyToggles[key];
	switch(key) {
		case 'h':
			scene->step();
			break;
		case 'r':
			scene->reset();
			break;
		case 'f':
			scene->free_fall();
			break;
	}
}

static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if(state == GLFW_PRESS) {
		camera->mouseMoved(xmouse, ymouse);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// Get the current mouse position.
	double xmouse, ymouse;
	glfwGetCursorPos(window, &xmouse, &ymouse);
	// Get current window size.
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if(action == GLFW_PRESS) {
		bool shift = mods & GLFW_MOD_SHIFT;
		bool ctrl  = mods & GLFW_MOD_CONTROL;
		bool alt   = mods & GLFW_MOD_ALT;
		camera->mouseClicked(xmouse, ymouse, shift, ctrl, alt);
	}
}

static void init()
{
	GLSL::checkVersion();
	
	// Set background color
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test
	glEnable(GL_DEPTH_TEST);
	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	progSimple = make_shared<Program>();
	progSimple->setShaderNames(RESOURCE_DIR + "simple_vert.glsl", RESOURCE_DIR + "simple_frag.glsl");
	progSimple->setVerbose(true); // Set this to true when debugging.
	progSimple->init();
	progSimple->addUniform("P");
	progSimple->addUniform("MV");
	progSimple->setVerbose(false);
	
	prog = make_shared<Program>();
	prog->setVerbose(true); // Set this to true when debugging.
	prog->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	prog->init();
	prog->addUniform("P");
	prog->addUniform("MV");
	prog->addUniform("kdFront");
	prog->addUniform("kdBack");
	prog->addAttribute("aPos");
	prog->addAttribute("aNor");
	prog->addAttribute("aTex");
	prog->setVerbose(false);
	
	camera = make_shared<Camera>();

	scene = make_shared<Scene>();
	scene->load(RESOURCE_DIR);
	scene->tare();
	scene->init(track_number);
	
	// If there were any OpenGL errors, this will print something.
	// You can intersperse this line in your code to find the exact location
	// of your OpenGL error.
	GLSL::checkError(GET_FILE_LINE);
}

void render()
{
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	
	// Use the window size for camera.
	glfwGetWindowSize(window, &width, &height);
	camera->setAspect((float)width/(float)height);
	
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(keyToggles[(unsigned)'c']) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}
	if(keyToggles[(unsigned)'z']) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();
	
	// Apply camera transforms
	P->pushMatrix();
	camera->applyProjectionMatrix(P);
	MV->pushMatrix();
	camera->applyViewMatrix(MV);

	// apply projection
	progSimple->bind();
	glUniformMatrix4fv(progSimple->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
	progSimple->unbind();

	// Draw scene
	prog->bind();
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
	MV->pushMatrix();
	scene->draw(MV, prog, progSimple);
	MV->popMatrix();
	prog->unbind();
	
	//////////////////////////////////////////////////////
	// Cleanup
	//////////////////////////////////////////////////////

	// Pop stacks
	MV->popMatrix();
	P->popMatrix();
	
	GLSL::checkError(GET_FILE_LINE);
}

void stepperFunc()
{
	double t = 0;
	int n = 0;
	while(!stop_flag) {
		auto t0 = std::chrono::system_clock::now();
		if(keyToggles[(unsigned)' ']) {
			scene->step();
		}
		auto t1 = std::chrono::system_clock::now();
		double dt = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
		t += dt*1e-3;
		n++;
		
		auto start = std::chrono::high_resolution_clock::now();
		while (std::chrono::high_resolution_clock::now() - start < std::chrono::microseconds(1)) {}

		if(t > 1000) {
			if(keyToggles[(unsigned)' '] && keyToggles[(unsigned)'t']) {
				cout << t/n << " ms/step" << endl;
			}
			t = 0;
			n = 0;
		}
	}
}

int main(int argc, char **argv)
{
	if(argc < 3) {
		cout << "Please specify the resource directory and the track number." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");
	track_number = atoi(argv[2]);
	
	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "Thomas Ducote", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	// Set char callback.
	glfwSetCharCallback(window, char_callback);
	// Set cursor position callback.
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// Set mouse button callback.
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	// Initialize scene.
	init();
	// Start simulation thread.
	stop_flag = false;
	thread stepperThread(stepperFunc);
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		if(!glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
			// Render scene.
			render();
			// Swap front and back buffers.
			glfwSwapBuffers(window);
		}
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	stop_flag = true;
	stepperThread.join();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
