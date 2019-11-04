#include<windows.h>
#include<stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>         /* glut.h 包含 gl.h和glu.h*/
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GLFW_INCLUDE_GLCOREARB 1

GLFWwindow *window;

GLuint shader_program;
GLuint VAO;
unsigned int texture1, texture2;

GLuint CreateShaderProgram();
GLuint LoadShader(const char *shader_path, GLenum shader_type);
void LinkShader(GLuint shader_pragram, ...);  /// 以0结束
bool compileShader(GLuint &shader_id, const char *path);

static const GLchar *getFileData(const char * path);

static const int MAX_LOG_LEN = 16 * 1024;

static void _log(const char *format, va_list args)
{
	char buf[MAX_LOG_LEN];
	vsnprintf(buf, MAX_LOG_LEN - 3, format, args);
	strcat_s(buf, "\n");

	WCHAR wszBuf[MAX_LOG_LEN] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, buf, -1, wszBuf, sizeof(wszBuf));
	OutputDebugStringW(wszBuf);
	WideCharToMultiByte(CP_ACP, 0, wszBuf, -1, buf, sizeof(buf), nullptr, FALSE);
	printf("%s", buf);
	fflush(stdout);
}

void log(const char *format, ...);


/**********
* loadshader.cpp
**********/
#include <fstream>
#include <vector>

GLuint LoadShader(const char *shader_path, GLenum shader_type)
{
	GLuint shader_id = glCreateShader(shader_type);

	bool bRet = compileShader(shader_id, shader_path);
	if (!bRet)
	{
		glDeleteShader(shader_id);
		return 0;
	}

	return shader_id;
}

static const GLchar* getFileData(const char * path)
{
	FILE* infile;
	fopen_s(&infile, path, "rb");
	if (!infile)
	{
		return NULL;
	}
	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	GLchar *source = new GLchar[len + 1];
	fread(source, 1, len, infile);
	fclose(infile);
	source[len] = 0;
	return const_cast<const GLchar *>(source);
}

bool compileShader(GLuint &shader_id, const char *path)
{
	const GLchar *shader_code = getFileData(path);
	if (strlen(shader_code) <= 0)
		return 0;

	GLint Result = GL_FALSE;
	GLint InfoLogLength;

	glShaderSource(shader_id, 1, &shader_code, NULL);
	glCompileShader(shader_id);
	delete[]shader_code;

	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &Result);
	if (!Result) {
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
		std::vector<char> ShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(shader_id, InfoLogLength, NULL, &ShaderErrorMessage[0]);
		//printf("%s\n", &ShaderErrorMessage[0]);
		log("%s", &ShaderErrorMessage[0]);
		return false;
	}

	return true;
}

void log(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	_log(format, args);
	va_end(args);
}

bool LoadTessellationShader(GLuint shader_program, const char *tes_shader_path)
{
	GLuint shader = glCreateShader(GL_TESS_CONTROL_SHADER);
	bool bRet = compileShader(shader, tes_shader_path);
	if (!bRet)
		return false;
	glAttachShader(shader_program, shader);
	glLinkProgram(shader_program);

	glDeleteProgram(shader);

	return true;
}

void LinkShader(GLuint shader_pragram, ...)
{
	// 省略参数的使用方法
	va_list args;  // 省略参数的参数列表
	va_start(args, shader_pragram);  // 初始化省略参数列表，第二个参数为省略参的前一个，用于记录省略参起始位置
	while (true)
	{
		// 将参数列表中的当前参数数值转换为对应的类型,这个宏在执行完后，会更改对象args，将其指向下一个
		GLuint shader_id = va_arg(args, GLuint);
		if (shader_id <= 0)
			break;
		glAttachShader(shader_pragram, shader_id);
		glDeleteShader(shader_id);
	}
	va_end(args); // 关闭本次省略参列表的访问
	glLinkProgram(shader_pragram);
}

GLuint CreateShaderProgram()
{
	return glCreateProgram();
}

void init()
{
	//static const GLfloat positions[] =
	//{
	//	-0.5f, -0.5f, 0.f,
	//	 0.5f, -0.5f, 0.f,
	//	 0.5f,  0.5f, 0.0f,
	//	-0.5f,  0.5f, 0.0f,
	//};

	//static const GLfloat colors[] =
	//{
	//	1.0f, 0.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f,
	//	0.0f, 0.0f, 1.0f,
	//	1.0f, 1.0f, 1.0f,
	//};

	//static const GLfloat uvs[] =
	//{
	//	0.0,0.0,
	//	1.0,0.0,
	//	1.0,1.0,
	//	0.0,1.0,
	//};

	//static const GLushort vertex_indices[] =
	//{
	//	0,3,2,
	//	0,1,2,
	//};


	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);

	//GLuint buffer;
	//glGenBuffers(1, &buffer);
	//glBindBuffer(GL_ARRAY_BUFFER, buffer);
	////glBindBuffer(GL_DRAW_INDIRECT_BUFFER, VAO);  //c
	//glBufferData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(colors) + sizeof(uvs), nullptr, GL_STATIC_DRAW);

	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), positions);
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(colors), colors);
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(colors), sizeof(uvs), uvs);

	//GLuint index_buffer;
	//glGenBuffers(1, &index_buffer);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

	float vertices[] = {
		// 位置信息          // 颜色信息           // 纹理 coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // 右上
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // 右下
		 -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // 左下
		 -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // 左上
	};
	GLushort indices[] = {
		0, 1, 3, // 第一个三角形
		1, 2, 3  // 第二个三角形
	};

	unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	shader_program = CreateShaderProgram();
	GLuint vec_shader = LoadShader("triangles.vert", GL_VERTEX_SHADER);
	GLuint fs_shader = LoadShader("triangles.frag", GL_FRAGMENT_SHADER);
	LinkShader(shader_program, vec_shader, fs_shader, 0);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	/* glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	 glEnableVertexAttribArray(0);
	 glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	 glEnableVertexAttribArray(1);
	 glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
	 glEnableVertexAttribArray(2);*/
	 //    加载并创建纹理
	 // -------------------------
	//    第一张纹理
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	//    为当前绑定的纹理对象设置环绕、过滤方式
	   //    将纹理包装设置为GL_REPEAT（默认包装方法）
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//    设置纹理过滤参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// 加载并生成纹理
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); //告诉stb_image.h在y轴上翻转加载的纹理。

	unsigned char *data = stbi_load("spp.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	// texture 2
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("feijishen.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		//请注意，awesomeface.png具有透明度，因此具有alpha通道，
	  //因此请务必告诉OpenGL数据类型为GL_RGBA        
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	//告诉每个采样器的opengl它属于哪个纹理单元（只需要做一次）
	glUseProgram(shader_program);
	// either set it manually like so:
	glUniform1i(glGetUniformLocation(shader_program, "texture1"), 0);
	// or set it via the texture class
	glUniform1i(glGetUniformLocation(shader_program, "texture2"), 1);

	glBindVertexArray(0);

	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
}

struct DrawArrysIndirectComnand
{
	GLuint count;
	GLuint primCount;
	GLuint first;
	GLuint baseInstance;
};

void display()
{
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		/*glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5);*/
		glUseProgram(shader_program);
		glBindVertexArray(VAO);
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		/*	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
			glEnableVertexAttribArray(1);*/
			//glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);  //顶点绘制方式
			//glFrontFace(GL_CCW);   // 设置裁剪面为背面,默认模式
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 多边形 渲染方式
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL); // 索引绘制方式
		//glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, 3, GL_UNSIGNED_SHORT, NULL, 1);// 索引绘制方式,可以对索引数据进行一个固定数量的偏移
		//glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0,3,1); 
		// 间接绘制函数，不是直接从程序中获取，而是从缓存对象中获取,使用前必须先将一个缓存对象绑定到GL_DRAW_INDIRECT_BUFFER 目标上
		//DrawArrysIndirectComnand cmd;
		//cmd.count = 3;
		//cmd.primCount = 1;
		//cmd.first = 0;
		//cmd.baseInstance = 0;
		//glDrawArraysIndirect(GL_TRIANGLE_STRIP, &cmd);
		glBindVertexArray(0);
		//glDisable(GL_ALPHA_TEST);

		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	glDeleteProgram(shader_program);
}

int CALLBACK WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nShowCmd)
{
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(512, 512, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (GLEW_OK != glewInit())
		return -1;

	init();

	display();

	return 0;
}