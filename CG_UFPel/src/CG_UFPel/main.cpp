#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#include <cmath>

#define N_MODELOS 3
#define N_CAMERAS 3
#define N_PASSOS_MODELO 4
#define N_PASSOS_CAMERA 4

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(Shader s, Model m, GLFWwindow *window);

// fun寤es modelo
void escala(Shader s, Model m, GLFWwindow* window, float tempo);
void translacao(Shader s, Model m, GLFWwindow* window, float tempo);
void bezier(Shader s, Model m, GLFWwindow* window, float tempo, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);
void rotacao(Shader s, Model m, GLFWwindow* window, float tempo);
void rotacaoPonto(Shader s, Model m, GLFWwindow* window, float tempo, glm::vec3 p);
void animacao(Shader s, Model m, GLFWwindow* window, float tempo);
// fun寤es camera
void translacaoCamera(Shader s, Model m, GLFWwindow* window, float tempo);
void bezierCamera(Shader s, Model m, GLFWwindow* window, float tempo, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);
void rotacaoCamera(Shader s, Model m, GLFWwindow* window, float tempo);
void rotacaoPontoCamera(Shader s, Model m, GLFWwindow* window, float tempo, glm::vec3 p);
void zoomCamera(Shader s, Model m, GLFWwindow* window, float tempo);
void animacaoCamera(Shader s, Model m, GLFWwindow* window, float tempo);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
int cameraAtual = 0;
Camera camera[N_CAMERAS] = { glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.75f, 0.25f, 5.0f), glm::vec3(-0.25f, 0.5f, 4.0f) };
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float currentFrame = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// MODELO
int modeloAtual = 0;
// escala inicial 0.05
glm::vec3 escalas[N_MODELOS] = { glm::vec3(0.05f), glm::vec3(0.05f), glm::vec3(0.05f) };
glm::vec3 pAtuais[N_MODELOS] = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(-0.5f, 0.0f, 0.0f) };

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader(FileSystem::getPath("resources/cg_ufpel.vs").c_str(), FileSystem::getPath("resources/cg_ufpel.fs").c_str());

    // load models
    // -----------
    Model ourModel(FileSystem::getPath("resources/objects/nanosuit/nanosuit.obj"));
    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        processInput(ourShader, ourModel, window);
        // render
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // don't forget to enable shader before setting uniforms
        ourShader.use();
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera[cameraAtual].Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera[cameraAtual].GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
		// render the loaded model
		for (int i = 0; i < N_MODELOS; i++) {
			glm::mat4 model;
			model = glm::translate(model, pAtuais[i]); // translate it down so it's at the center of the scene
			model = glm::scale(model, escalas[i]);	// it's a bit too big for our scene, so scale it down

			ourShader.setMat4("model", model);
			ourModel.Draw(ourShader);
		}
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void animacao(Shader s, Model m, GLFWwindow* window, float tempo) {
	// per-frame time logic
	int passosRestantes = N_PASSOS_MODELO;
	float inicio[N_PASSOS_MODELO] = { 0.0f, 0.0f, 0.0f, 0.0f };
	currentFrame = glfwGetTime();
	inicio[0] = currentFrame;
	glm::vec3 pInicial = pAtuais[modeloAtual];

	// 1
	inicio[0] = tempo - (glfwGetTime() - inicio[0]);
	printf("tempo: %f passos restantes: %d\n", inicio[0], passosRestantes);
	translacao(s, m, window, (float)(inicio[0] / passosRestantes));
	passosRestantes--;
	// 2
	inicio[1] = tempo - (glfwGetTime() - inicio[0]);
	printf("tempo: %f %f %f\n", tempo, glfwGetTime(), inicio[0]);
	printf("tempo: %f passos restantes: %d\n", inicio[1], passosRestantes);
	bezier(s, m, window, (float)(inicio[1] / passosRestantes), pInicial, glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(1.5f, 1.5f, 0.0f));
	passosRestantes--;
	// 3
	inicio[2] = tempo - (glfwGetTime() - inicio[0]);
	printf("tempo: %f %f %f\n", tempo, glfwGetTime(), inicio[0]);
	printf("tempo: %f passos restantes: %d\n", inicio[2], passosRestantes);
	rotacao(s, m, window, (float)(inicio[2] / passosRestantes));
	passosRestantes--;
	// 4
	inicio[3] = tempo - (glfwGetTime() - inicio[0]);
	printf("tempo: %f %f %f\n", tempo, glfwGetTime(), inicio[0]);
	printf("tempo: %f passos restantes: %d\n", inicio[3], passosRestantes);
	translacao(s, m, window, (float)(inicio[3] / passosRestantes));	
}

// rotacao
void rotacaoPonto(Shader s, Model m, GLFWwindow* window, float tempo, glm::vec3 p) {
	// per-frame time logic
	float inicio = glfwGetTime();
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - inicio;

	float angulo;

	while (deltaTime <= tempo) {
		angulo = (float)((deltaTime * 360.f) / tempo);

		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		s.use();

		// render the loaded model
		for (int i = 0; i < N_MODELOS; i++) {
			glm::mat4 model;
			if (i == modeloAtual) {
				model = glm::translate(model, p);
				model = glm::rotate(model, glm::radians(angulo), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::translate(model, p);
			}
			else
				model = glm::translate(model, pAtuais[i]);
			model = glm::scale(model, escalas[i]);	// it's a bit too big for our scene, so scale it down

			s.setMat4("model", model);
			m.Draw(s);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - inicio;
	}
}

// rotacao
void rotacao(Shader s, Model m, GLFWwindow* window, float tempo) {
	// per-frame time logic
	float inicio = glfwGetTime();
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - inicio;

	float angulo;

	while (deltaTime <= tempo) {
		angulo = (float)((deltaTime * 360.f) / tempo);

		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		s.use();

		// render the loaded model
		for (int i = 0; i < N_MODELOS; i++) {
			glm::mat4 model;
			model = glm::translate(model, pAtuais[i]);
			if (i == modeloAtual) {
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			model = glm::scale(model, escalas[i]);	// it's a bit too big for our scene, so scale it down

			s.setMat4("model", model);
			m.Draw(s);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - inicio;
	}
}

// bezier
void bezier(Shader s, Model m, GLFWwindow* window, float tempo, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
	float inicio = glfwGetTime();
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - inicio;
	float t = (float) deltaTime / tempo;

	while (deltaTime <= tempo) {
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		s.use();

		printf("t: %f\n", t);
		pAtuais[modeloAtual].x = pow(1 - t, 2) * p0.x +
			(1 - t) * 2 * t * p1.x +
			t * t * p2.x;
		printf("x: %f\n", pAtuais[modeloAtual].x);

		pAtuais[modeloAtual].y = pow(1 - t, 2) * p0.y +
			(1 - t) * 2 * t * p1.y +
			t * t * p2.y;
		printf("y: %f\n", pAtuais[modeloAtual].y);

		for (int i = 0; i < N_MODELOS; i++) {
			glm::mat4 model;
			model = glm::translate(model, pAtuais[i]);
			model = glm::scale(model, escalas[i]);

			s.setMat4("model", model);
			m.Draw(s);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - inicio;
		t = deltaTime * 0.1f;
	}
}

// translacao linear
void translacao(Shader s, Model m, GLFWwindow* window, float tempo) {
	// per-frame time logic
	float inicio = glfwGetTime();
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - inicio;
	glm::vec3 pInicial = pAtuais[modeloAtual];

	while (deltaTime <= tempo) {
		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		s.use();

		pAtuais[modeloAtual].x = pInicial.x + (float)deltaTime * 0.1;

		// render the loaded model
		for (int i = 0; i < N_MODELOS; i++) {
			glm::mat4 model;
			model = glm::translate(model, pAtuais[i]); // translate it down so it's at the center of the scene
			model = glm::scale(model, escalas[i]);	// it's a bit too big for our scene, so scale it down

			s.setMat4("model", model);
			m.Draw(s);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - inicio;
	}
}

// escala
void escala(Shader s, Model m, GLFWwindow* window, float tempo) {
	// per-frame time logic
	float inicio = glfwGetTime();
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - inicio;

	while (deltaTime <= tempo) {
		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		s.use();

		escalas[modeloAtual] += (float)deltaTime * 0.0001;

		// render the loaded model
		for (int i = 0; i < N_MODELOS; i++) {
			glm::mat4 model;
			model = glm::translate(model, pAtuais[i]); // translate it down so it's at the center of the scene
			model = glm::scale(model, escalas[i]);	// it's a bit too big for our scene, so scale it down

			s.setMat4("model", model);
			m.Draw(s);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - inicio;
	}
}

// FUN합ES CAMERA
// animacao
void animacaoCamera(Shader s, Model m, GLFWwindow* window, float tempo) {
	// per-frame time logic
	int passosRestantes = N_PASSOS_CAMERA;
	float inicio[N_PASSOS_CAMERA] = { 0.0f, 0.0f, 0.0f, 0.0f };
	currentFrame = glfwGetTime();
	inicio[0] = currentFrame;
	glm::vec3 pInicial = pAtuais[modeloAtual];

	// 1
	inicio[0] = tempo - (glfwGetTime() - inicio[0]);
	printf("tempo: %f passos restantes: %d\n", inicio[0], passosRestantes);
	translacao(s, m, window, (float)(inicio[0] / passosRestantes));
	passosRestantes--;
	// 2
	inicio[1] = tempo - (glfwGetTime() - inicio[0]);
	printf("tempo: %f %f %f\n", tempo, glfwGetTime(), inicio[0]);
	printf("tempo: %f passos restantes: %d\n", inicio[1], passosRestantes);
	bezier(s, m, window, (float)(inicio[1] / passosRestantes), pInicial, glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(1.5f, 1.5f, 0.0f));
	passosRestantes--;
	// 3
	inicio[2] = tempo - (glfwGetTime() - inicio[0]);
	printf("tempo: %f %f %f\n", tempo, glfwGetTime(), inicio[0]);
	printf("tempo: %f passos restantes: %d\n", inicio[2], passosRestantes);
	rotacao(s, m, window, (float)(inicio[2] / passosRestantes));
	passosRestantes--;
	// 4
	inicio[3] = tempo - (glfwGetTime() - inicio[0]);
	printf("tempo: %f %f %f\n", tempo, glfwGetTime(), inicio[0]);
	printf("tempo: %f passos restantes: %d\n", inicio[3], passosRestantes);
	translacao(s, m, window, (float)(inicio[3] / passosRestantes));
}

// zoom
void zoomCamera(Shader s, Model m, GLFWwindow* window, float tempo) {
	// per-frame time logic
	float inicio = glfwGetTime();
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - inicio;

	float yoffset;

	while (deltaTime <= tempo) {
		yoffset = (float)((deltaTime * 5.0f) / tempo);

		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		s.use();

		if (camera[cameraAtual].Zoom >= 1.0f && camera[cameraAtual].Zoom <= 45.0f)
			camera[cameraAtual].Zoom -= yoffset;
		if (camera[cameraAtual].Zoom <= 1.0f)
			camera[cameraAtual].Zoom = 1.0f;
		if (camera[cameraAtual].Zoom >= 45.0f)
			camera[cameraAtual].Zoom = 45.0f;

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera[cameraAtual].Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera[cameraAtual].GetViewMatrix();
		s.setMat4("projection", projection);
		s.setMat4("view", view);

		// render the loaded model
		for (int i = 0; i < N_MODELOS; i++) {
			glm::mat4 model;
			model = glm::translate(model, pAtuais[i]);
			model = glm::scale(model, escalas[i]);	// it's a bit too big for our scene, so scale it down

			s.setMat4("model", model);
			m.Draw(s);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - inicio;
	}
}

// rotacao num ponto
void rotacaoPontoCamera(Shader s, Model m, GLFWwindow* window, float tempo, glm::vec3 p) {
	// per-frame time logic
	float inicio = glfwGetTime();
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - inicio;

	float angulo;

	while (deltaTime <= tempo) {
		angulo = (float)((deltaTime * 360.f) / tempo);

		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		s.use();

		camera[cameraAtual].Position = p;

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera[cameraAtual].Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera[cameraAtual].GetViewMatrix();
		view = glm::rotate(view, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		s.setMat4("projection", projection);
		s.setMat4("view", view);

		// render the loaded model
		for (int i = 0; i < N_MODELOS; i++) {
			glm::mat4 model;
			model = glm::translate(model, pAtuais[i]);
			model = glm::scale(model, escalas[i]);	// it's a bit too big for our scene, so scale it down

			s.setMat4("model", model);
			m.Draw(s);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - inicio;
	}
}

// rotacao
void rotacaoCamera(Shader s, Model m, GLFWwindow* window, float tempo) {
	// per-frame time logic
	float inicio = glfwGetTime();
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - inicio;

	float angulo;

	while (deltaTime <= tempo) {
		angulo = (float)((deltaTime * 360.f) / tempo);

		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		s.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera[cameraAtual].Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera[cameraAtual].GetViewMatrix();
		view = glm::rotate(view, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		s.setMat4("projection", projection);
		s.setMat4("view", view);

		// render the loaded model
		for (int i = 0; i < N_MODELOS; i++) {
			glm::mat4 model;
			model = glm::translate(model, pAtuais[i]);
			model = glm::scale(model, escalas[i]);	// it's a bit too big for our scene, so scale it down

			s.setMat4("model", model);
			m.Draw(s);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - inicio;
	}
}

// bezier
void bezierCamera(Shader s, Model m, GLFWwindow* window, float tempo, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
	float inicio = glfwGetTime();
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - inicio;
	float t = (float)deltaTime / tempo;

	while (deltaTime <= tempo) {
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		s.use();

		printf("t: %f\n", t);
		camera[cameraAtual].Position.x = pow(1 - t, 2) * p0.x +
			(1 - t) * 2 * t * p1.x +
			t * t * p2.x;
		printf("x: %f\n", camera[cameraAtual].Position.x);

		camera[cameraAtual].Position.y = pow(1 - t, 2) * p0.y +
			(1 - t) * 2 * t * p1.y +
			t * t * p2.y;
		printf("y: %f\n", camera[cameraAtual].Position.y);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera[cameraAtual].Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera[cameraAtual].GetViewMatrix();
		s.setMat4("projection", projection);
		s.setMat4("view", view);

		for (int i = 0; i < N_MODELOS; i++) {
			glm::mat4 model;
			model = glm::translate(model, pAtuais[i]);
			model = glm::scale(model, escalas[i]);

			s.setMat4("model", model);
			m.Draw(s);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - inicio;
		t = deltaTime * 0.1f;
	}
}

// translacao linear camera
void translacaoCamera(Shader s, Model m, GLFWwindow* window, float tempo) {
	// per-frame time logic
	float inicio = glfwGetTime();
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - inicio;

	while (deltaTime <= tempo) {
		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		s.use();

		camera[cameraAtual].Position.x += (float)deltaTime * 0.01;

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera[cameraAtual].Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera[cameraAtual].GetViewMatrix();
		s.setMat4("projection", projection);
		s.setMat4("view", view);

		// render the loaded model
		for (int i = 0; i < N_MODELOS; i++) {
			glm::mat4 model;
			model = glm::translate(model, pAtuais[i]); // translate it down so it's at the center of the scene
			model = glm::scale(model, escalas[i]);	// it's a bit too big for our scene, so scale it down

			s.setMat4("model", model);
			m.Draw(s);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - inicio;
	}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(Shader s, Model m, GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
		camera[cameraAtual].ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
		camera[cameraAtual].ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
		camera[cameraAtual].ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
		camera[cameraAtual].ProcessKeyboard(RIGHT, deltaTime);
	// TROCA MODELOS
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		modeloAtual = (int)((modeloAtual + 1) % N_MODELOS);
		Sleep(500.0f);
	}
	// TROCA CAMERAS
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		cameraAtual = (int)((cameraAtual + 1) % N_CAMERAS);
		Sleep(500.0f);
	}
	// MODELOS
	// TRANSLA플O LINEAR EIXO X
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		translacao(s, m, window, 5.0f);
	// BEZIER QUADR햀ICO
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		bezier(s, m, window, 5.0f, pAtuais[modeloAtual], glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(1.5f, 1.5f, 0.0f));
	// ROTA플O
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		rotacao(s, m, window, 5.0f);
	// ROTA플O NUM PONTO
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		rotacaoPonto(s, m, window, 10.0f, glm::vec3(0.25f, 0.0f, 0.0f));
	// ESCALA
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		escala(s, m, window, 1.0f);
	// ANIMA플O
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		animacao(s, m, window, 10.0f);
	// ---------------------------------------------------------------------------------------------
	// CAMERAS
	// TRANSLA플O LINEAR EIXO X
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		translacaoCamera(s, m, window, 2.0f);
	// BEZIER QUADR햀ICO
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		bezierCamera(s, m, window, 5.0f, camera[cameraAtual].Position, glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(1.5f, 1.5f, 0.0f));
	// ROTA플O
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		rotacaoCamera(s, m, window, 5.0f);
	// ROTA플O NUM PONTO
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		rotacaoPontoCamera(s, m, window, 10.0f, glm::vec3(-0.25f, 0.5f, 4.0f));
	// ZOOM
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		zoomCamera(s, m, window, 0.25f);
	// ANIMA플O
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		animacaoCamera(s, m, window, 10.0f);
	// ---------------------------------------------------------------------------------------------
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

	camera[cameraAtual].ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera[cameraAtual].ProcessMouseScroll(yoffset);
}
