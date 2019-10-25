/*
ZJ Wood CPE 471 Lab 3 base code
*/

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;
double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		pos = glm::vec3(0, 0, -15);
		rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process(double frametime)
	{
		double ftime = frametime;
		float speed = 0;
		if (w == 1)
		{
			speed = 10*ftime;
		}
		else if (s == 1)
		{
			speed = -10*ftime;
		}
		float yangle=0;
		if (a == 1)
			yangle = -1*ftime;
		else if(d==1)
			yangle = 1*ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;

class Application : public EventCallbacks
{

public:
	int kn = 0;
	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog, shapeprog;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID, VAO_CYL;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexColorIDBox, IndexBufferIDBox, VBO_CYL_ID;

	Shape shape;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
		if (key == GLFW_KEY_N && action == GLFW_PRESS) kn = 1;
		if (key == GLFW_KEY_N && action == GLFW_RELEASE) kn = 0;
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	glm::vec3* generateCylinderPositions(int size)
	{
		int i;
		float t = 0, dt = 2.0 * 3.1415926 / (float)size, r = 0.8, length = 2;
		glm::vec3* positions = new glm::vec3[size * 12];

		for (i = 0; i < size; i++) {
			positions[i * 12 + 0] = glm::vec3(r * cos(t), r * sin(t), length);
			positions[i * 12 + 1] = glm::vec3(0, 0, length);
			t += dt;
			positions[i * 12 + 2] = glm::vec3(r * cos(t), r * sin(t), length);

			positions[i * 12 + 3] = glm::vec3(r * cos(t), r * sin(t), length);
			positions[i * 12 + 4] = glm::vec3(r * cos(t), r * sin(t), -length);
			t -= dt;
			positions[i * 12 + 5] = glm::vec3(r * cos(t), r * sin(t), length);

			positions[i * 12 + 6] = glm::vec3(r * cos(t), r * sin(t), length);
			positions[i * 12 + 7] = glm::vec3(r * cos(t), r * sin(t), -length);
			t += dt;
			positions[i * 12 + 8] = glm::vec3(r * cos(t), r * sin(t), -length);

			positions[i * 12 + 9] = glm::vec3(r * cos(t), r * sin(t), -length);
			t -= dt;
			positions[i * 12 + 10] = glm::vec3(r * cos(t), r * sin(t), -length);
			positions[i * 12 + 11] = glm::vec3(0, 0, -length);

			t += dt;
		}

		/*for (i = 0; i < size * 12; i++){
			//std::cout << "x: " << positions[i].x << " y: " << positions[i].y << " z: " << positions[i].z << std::endl;
			positions[i].x += 5;
		}*/
		return positions;
	}

	glm::vec3* generateCylinderColors(int size)
	{
		int i;
		glm::vec3* colors = new glm::vec3[size * 12];
		for (i = 0; i < size * 12; i++) {
			if (i % 12 >= 3 && i % 12 <= 8) {
				colors[i] = glm::vec3(0.0f, 0.0f, 1.0f);
			}
			else {
				colors[i] = glm::vec3(1.0f, 1.0f, 0.0f);
			}		
		}
		return colors;
	}


	void initCylinder()
	{
		int size = 50;

		glGenVertexArrays(1, &VAO_CYL);
		glBindVertexArray(VAO_CYL);

		//positions
		glGenBuffers(1, &VBO_CYL_ID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_CYL_ID);

		glm::vec3* cylinder_positions = generateCylinderPositions(size);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*size*36, cylinder_positions, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//colors
		glGenBuffers(1, &VBO_CYL_ID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_CYL_ID);

		glm::vec3* cylinder_colors = generateCylinderColors(size);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size * 36, cylinder_colors, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//normals
		glGenBuffers(1, &VBO_CYL_ID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_CYL_ID);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size * 36, cylinder_positions, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		delete(cylinder_positions);
		delete(cylinder_colors);

		glBindVertexArray(0);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		string resourceDirectory = "../resources";
		//try t800.obj or F18.obj ...
		shape.loadMesh(resourceDirectory + "/sphere.obj");
		shape.resize();
		shape.init();

		initCylinder();

		glBindVertexArray(0);

	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		// Enable blending/transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
			{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1); //make a breakpoint here and check the output window for the error message!
			}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertColor");
		prog->addAttribute("vertNor");

		// Initialize the GLSL program.
		shapeprog = std::make_shared<Program>();
		shapeprog->setVerbose(true);
		shapeprog->setShaderNames(resourceDirectory + "/shape_vertex.glsl", resourceDirectory + "/shape_fragment.glsl");
		if (!shapeprog->init())
			{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1); //make a breakpoint here and check the output window for the error message!
			}
		shapeprog->addUniform("P");
		shapeprog->addUniform("V");
		shapeprog->addUniform("M");
		shapeprog->addAttribute("vertPos");
		shapeprog->addAttribute("vertNor");
		shapeprog->addAttribute("vertTex");
	}

	glm::mat4 renderBody(mat4 P, mat4 V, mat4 M_parent, float w)
	{
		mat4 M = mat4(1);
		shapeprog->bind();
		glUniformMatrix4fv(shapeprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(shapeprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);

		glm::mat4 T_por = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0, 0, 0));
		glm::mat4 R1 = glm::rotate(glm::mat4(1.0f), 0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 R = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 T_attach = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(2, 1, 1));
		glm::mat4 M_body = M_parent * T_attach * R * R1 * T_por;
		M = M_body * S;

		glUniformMatrix4fv(shapeprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(shapeprog);
		shapeprog->unbind();
		return M_body;
	}

	glm::mat4 renderTail(mat4 P, mat4 V, mat4 M_parent)
	{
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glBindVertexArray(VAO_CYL);

		//send the matrices to the shaders
		mat4 T_por = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.2f));
		mat4 R = glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		mat4 T_attach = glm::translate(glm::mat4(1.0f), glm::vec3(3.75f, 0.0f, 0.0f));
		mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.18f, 0.18f, 1.0f));
		glm::mat4 M_tail = M_parent * T_attach * R * T_por;
		mat4 M = M_tail * S;

		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 50 * 36);
		prog->unbind();

		shapeprog->bind();
		glUniformMatrix4fv(shapeprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(shapeprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);

		T_por = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		R = glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		T_attach = glm::translate(glm::mat4(1.0f), glm::vec3(-0.2, 0, 2.0));
		S = glm::scale(glm::mat4(1.0f), glm::vec3(0.6, 0.6, 0.1));
		mat4 M_rutter = M_tail * T_attach * R * T_por;
		M = M_rutter * S;

		glUniformMatrix4fv(shapeprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(shapeprog);
		shapeprog->unbind();
		return M_tail;
	}

	void renderSkirts(mat4 P, mat4 V, mat4 M_parent)
	{
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glBindVertexArray(VAO_CYL);

		mat4 T_por = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		mat4 R = glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		mat4 T_attach = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.4f, 1.4f));
		mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.12f, 0.12f, 0.8f));
		glm::mat4 M_s1 = M_parent * T_attach * R * T_por;
		mat4 M = M_s1 * S;

		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 50 * 36);

		T_por = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		R = glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		T_attach = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.4f, -1.4f));
		S = glm::scale(glm::mat4(1.0f), glm::vec3(0.12f, 0.12f, 0.8f));
		glm::mat4 M_s2 = M_parent * T_attach * R * T_por;
		M = M_s2 * S;

		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 50 * 36);
		prog->unbind();
	}

	glm::mat4 renderBlade(mat4 P, mat4 V, mat4 M_parent)
	{
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glBindVertexArray(VAO_CYL);

		//send the matrices to the shaders
		mat4 T_por = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		mat4 R = glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(1.0f, 0.0f, 0.0f));
		mat4 T_attach = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.2f, 0.0f));
		mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.08f, 0.08f, 0.15f));
		glm::mat4 M_axel = M_parent * T_attach * T_por;
		mat4 M = M_axel * R * S;

		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 50 * 36);
		prog->unbind();

		shapeprog->bind();
		glUniformMatrix4fv(shapeprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(shapeprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);

		T_por = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		R = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		T_attach = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.5, 0));
		S = glm::scale(glm::mat4(1.0f), glm::vec3(1.8, 0.18, 1.8));
		mat4 M_blade = M_axel * T_attach * R * T_por;
		M = M_blade * S;

		glUniformMatrix4fv(shapeprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(shapeprog);
		shapeprog->unbind();

		return M_blade;
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{

		double frametime = get_last_elapsed_time();
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = glm::mat4(1);
		M = glm::mat4(1);
		P = glm::mat4(1);
		 //Apply orthographic projection....
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

		//animation with the model matrix:
		static float w = 0.0;
		//w += 0.03;//rotation angle
		static float t = 0;
		t += 0.01;
		float trans = 0;// sin(t) * 2;

		V = mycam.process(frametime);

		mat4 M_body = renderBody(P, V, mat4(1), w);	
		mat4 M_tail = renderTail(P, V, M_body);
		mat4 M_blade = renderBlade(P, V, M_body);
		renderSkirts(P, V, M_body);
	}

};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
