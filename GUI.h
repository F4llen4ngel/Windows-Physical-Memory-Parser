#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <utility>

class GUI {
public:
	GUI();

	void Prepare();
	void Render();
	bool IsMinimized();
	bool WindowShouldClose();
	GLFWwindow* GetWindow();
private:
	GLFWwindow* window = NULL;
};

