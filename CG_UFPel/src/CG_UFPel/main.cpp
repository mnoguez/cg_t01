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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// funções
void escala(Shader s, Model m, GLFWwindow* window, float tempo);
void translacao(Shader s, Model m, GLFWwindow* window, float tempo);
void bezier(Shader s, Model m, GLFWwindow* window, float tempo, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float currentFrame = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// MODELO
// escala inicial 0.05
float gscale = 0.05f;
// coordenadas iniciais 0.0f, -1.0f, 0.0f
glm::vec3 pAtual = glm::vec3(0.0f, -1.0f, 0.0f);

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
        processInput(window);

        // render
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model;
		model = glm::translate(model, pAtual); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(gscale));	// it's a bit too big for our scene, so scale it down

        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

		// ESCALA
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			escala(ourShader, ourModel, window, 1.0f);
		}
		// TRANSLAÇÂO LINEAR EIXO X
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
			translacao(ourShader, ourModel, window, 5.0f);
		}
		// BEZIER QUADRÁTICO
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
			bezier(ourShader, ourModel, window, 5.0f, pAtual, glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(1.5f, 1.5f, 0.0f));
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

// bezier
void bezier(Shader s, Model m, GLFWwindow* window, float tempo, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
	// per-frame time logic
	float inicio = glfwGetTime();
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - inicio;
	float t = (float) deltaTime / tempo;

	while (deltaTime <= tempo) {
		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		s.use();

		printf("t: %f\n", t);

		pAtual.x = pow(1 - t, 2) * p0.x +
			(1 - t) * 2 * t * p1.x +
			t * t * p2.x;

		printf("x: %f\n", pAtual.x);

		pAtual.y = pow(1 - t, 2) * p0.y +
			(1 - t) * 2 * t * p1.y +
			t * t * p2.y;

		printf("y: %f\n", pAtual.y);

		glm::mat4 model;
		model = glm::translate(model, pAtual);
		model = glm::scale(model, glm::vec3(gscale));

		s.setMat4("model", model);
		m.Draw(s);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
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
	glm::vec3 pInicial = pAtual;

	while (deltaTime <= tempo) {
		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		s.use();

		pAtual.x = pInicial.x + (float)deltaTime * 0.1;

		glm::mat4 model;
		model = glm::translate(model, pAtual);
		model = glm::scale(model, glm::vec3(gscale));

		s.setMat4("model", model);
		m.Draw(s);

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

		gscale += (float)deltaTime * 0.001;

		glm::mat4 model;
		model = glm::translate(model, pAtual);
		model = glm::scale(model, glm::vec3(gscale));

		s.setMat4("model", model);
		m.Draw(s);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - inicio;
	}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
