#include "utils/render.hpp"
#include "utils/shader.hpp"

int main(){

	//Start the subsystem and make the window. Also make a scope for the window so the window destroys after the main loop closes
	Render::startRender();

	//Set opengl attributes
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	//Create the window with resizeable flag
	SDL_Window* window = SDL_CreateWindow("3D Renderer", Render::windowWidth, Render::windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (!window) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window was not created correctly!");
		return 1;
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (!glContext) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Renderer was not created correctly!");
		return 1;
	}

	/*GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);*/

	int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
	printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	Render::isRunning = true;

	//Having a vertex buffer for 3 vectors for a triangle, X/Y/Z
	static const GLfloat vertexBufferData[] = {
		-0.5f, -0.5f, 0.0f, //First vertice at the bottom left
		0.5f, -0.5f, 0.0f, //Second vertice at the bottom right
		0.0f,  0.5f, 0.0f, //Third vertice at the middle top of the screen
	};

	//First, create the variable for the vertex buffer then put turn that variable into a vertex buffer
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	//This will talk about our vertex buffer??? im not sure what this means
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	//Give the vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);
	//Compile shader to make triangle red
	GLuint shaderID = Shader::LoadShaders("shader files/testVertexShader.glsl", "shader files/testFragmentShader.glsl");

	while (Render::isRunning) {
		//Event Polling
		std::optional<SDL_Event> event;
		while (event = Render::pollEvent()) //Make sure to poll ALL events
		{
			switch (event->type) {
			case SDL_EventType::SDL_EVENT_QUIT: //check if it the close window event
					Render::isRunning = false;
					break;
			}
		}

		//Render stuff
		// 
		// Clear the screen and set draw color
		glClearColor(155.0f / 255.0f, 155.0f / 255.0f, 155.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw stuff here
		//Drawing the triangle
		//Use shader
		try {
			glUseProgram(shaderID);
		}
		catch(std::exception &exception){
			SDL_LogCritical(SDL_LogCategory::SDL_LOG_CATEGORY_ERROR, exception.what());
		}
		
		//Enables vertex attribute array
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); //Bind a named buffer object to a specific buffer
		glVertexAttribPointer(
			0,                  //Set attribute to 0, no reason but make sure to match the layout in the shader
			3,                  //Number of components
			GL_FLOAT,           //Specifies the data type of each item in the array
			GL_FALSE,           //Should we normalize it?
			0,                  //Offset between consecutive vertex attributes
			(void*)0            //Offset of the first component in the array
		);
		//Finally draw the triangle
		glDrawArrays(GL_TRIANGLES, 0, 3); //Starting from vertex 0 draw the triangle
		glDisableVertexAttribArray(0); //Disables the array

		//Update the screen
		SDL_GL_SwapWindow(window);


	}
	
	//End the subsystem and destroy renderer and window
	SDL_GL_DestroyContext(glContext);
	SDL_DestroyWindow(window);
	Render::endRender();

	return 0;
}