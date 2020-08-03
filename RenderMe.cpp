#include <GL\glew.h>
#include <GL\freeglut.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <iostream>
using namespace std;
#pragma comment(lib, "glew32.lib")

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::perspective;

glm::mat4 ModelViewMatrix, ProjectionMatrix ,MVP;
glm::mat4 NormalMatrix;

int num = 0;
GLfloat vertex[10240 * 3][4];
GLfloat normal[10240 * 3][4];
char buf[1024];
static GLuint shaderID;
int Method=0;

void triangles(int level, GLfloat a[3], GLfloat b[3], GLfloat c[3]) {
	if (num >= sizeof(vertex) / sizeof(vertex[0])) {
		printf("vertices buffer overflow...\n");
		return;
	}
	if (level <= 0) {
		vertex[num][0] = a[0]; vertex[num][1] = a[1]; vertex[num][2] = a[2]; vertex[num][3] = 1;
		normal[num][0] = a[0]; normal[num][1] = a[1]; normal[num][2] = a[2]; normal[num][3] = 1;
		num++;
		vertex[num][0] = b[0]; vertex[num][1] = b[1]; vertex[num][2] = b[2]; vertex[num][3] = 1;
		normal[num][0] = b[0]; normal[num][1] = b[1]; normal[num][2] = b[2]; normal[num][3] = 1;
		num++;
		vertex[num][0] = c[0]; vertex[num][1] = c[1]; vertex[num][2] = c[2]; vertex[num][3] = 1;
		normal[num][0] = c[0]; normal[num][1] = c[1]; normal[num][2] = c[2]; normal[num][3] = 1;
		num++;
	} else {
		GLfloat ab[3];
		GLfloat bc[3];
		GLfloat ca[3];
		GLfloat l;
		// ab = (a + b) /2
		ab[0] = (a[0] + b[0]) / 2.0f; ab[1] = (a[1] + b[1]) / 2.0f; ab[2] = (a[2] + b[2]) / 2.0f;
		l = sqrtf(ab[0] * ab[0] + ab[1] * ab[1] + ab[2] * ab[2]);
		ab[0] /= l; ab[1] /= l; ab[2] /= l;
		// bc = (b + c) /2
		bc[0] = (b[0] + c[0]) / 2.0f; bc[1] = (b[1] + c[1]) / 2.0f; bc[2] = (b[2] + c[2]) / 2.0f;
		l = sqrtf(bc[0] * bc[0] + bc[1] * bc[1] + bc[2] * bc[2]);
		bc[0] /= l; bc[1] /= l; bc[2] /= l;
		// ca = (c + a) /2
		ca[0] = (c[0] + a[0]) / 2.0f; ca[1] = (c[1] + a[1]) / 2.0f; ca[2] = (c[2] + a[2]) / 2.0f;
		l = sqrtf(ca[0] * ca[0] + ca[1] * ca[1] + ca[2] * ca[2]);
		ca[0] /= l; ca[1] /= l; ca[2] /= l;
		// call sub-triangles
		triangles(level - 1, a, ab, ca);
		triangles(level - 1, b, bc, ab);
		triangles(level - 1, c, ca, bc);
		triangles(level - 1, ab, bc, ca);
	}
}
void generateSphere(int level) {
	static GLfloat v[4][3] = {
		0.0f, 0.0f, 1.0f,
		0.943f, 0.0f, -0.333f,
		-0.471f, 0.816f, -0.333f,
		-0.471f, -0.816f, -0.333f,
	};
	num = 0;
	triangles(level, v[0], v[1], v[2]);
	triangles(level, v[0], v[2], v[3]);
	triangles(level, v[0], v[3], v[1]);
	triangles(level, v[3], v[2], v[1]);
}
void setLK(void)
{
	GLuint loc;
	vec4 Position = glm::vec4(1.5f, 1.5f, -2.0f, 1.0f);
	vec3 La=vec3(0.7f,0.7f,0.7f);
	vec3 Ld=vec3(0.6f,0.9f,0.9f);
	vec3 Ls=vec3(0.6f,1.0f,1.0f);
	float Shininess= 50.0f;
	vec3 Ka=vec3(0.1f,0.4f,0.3f);
	vec3 Kd=vec3(0.1f,0.9f,0.6f);
	vec3 Ks=vec3(0.5f,1.0f,0.0f);

	loc=glGetUniformLocation(shaderID,"Position");
	glUniform4fv(loc,1,&Position[0]);
	loc=glGetUniformLocation(shaderID,"La");
	glUniform3fv(loc,1,&La[0]);
	loc=glGetUniformLocation(shaderID,"Ld");
	glUniform3fv(loc,1,&Ld[0]);
	loc=glGetUniformLocation(shaderID,"Ls");
	glUniform3fv(loc,1,&Ls[0]);
	loc=glGetUniformLocation(shaderID,"Shininess");
	glUniform1f(loc,Shininess);
	loc=glGetUniformLocation(shaderID,"Ka");
	glUniform3fv(loc,1,&Ka[0]);
	loc=glGetUniformLocation(shaderID,"Kd");
	glUniform3fv(loc,1,&Kd[0]);
	loc=glGetUniformLocation(shaderID,"Ks");
	glUniform3fv(loc,1,&Ks[0]);
}
void setMatrices(void)
{
	glm::mat4 view = glm::lookAt( glm::vec3(0.0,0.0,15.0),glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,1.0,0.0) );
	glm::mat4 model = glm::mat4();
	model[0][0] = model[1][1] = model[2][2] = 0.001;
	glm::mat4 ModelViewMatrix = view * model;
	glm::mat4 NormalMatrix = transpose(inverse(ModelViewMatrix));
	glm::mat4 ProjectionMatrix= glm::perspective(45.0f,1.0f,0.1f,1000.0f);
	glm::mat4 MVP= ProjectionMatrix * ModelViewMatrix;
}
static char* readfile(const char* filename)
{
	FILE* fp= fopen(filename,"r");
	fseek(fp, 0,SEEK_END);
	long file_length =ftell(fp);
	fseek(fp,0,SEEK_SET);
	char * contents = new char[file_length+1];
	for(int i=0;i<file_length+1;i++)
	{
		contents[i]=0;
	}
	fread(contents,1,file_length,fp);
	contents[file_length+1]='\0';
	fclose(fp);
	return contents;
}
GLuint createVertexShader(const char * sourceCode)
{
	GLint status;
	GLuint vertexShaderID=glCreateShader(GL_VERTEX_SHADER);
	if( 0 == vertexShaderID )
	{
	  fprintf(stderr, "Error creating vertex shader.\n");
	  exit(1);
	}
	glShaderSource(vertexShaderID,1,(const GLchar **)&sourceCode,NULL);
	glCompileShader(vertexShaderID);
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &status);
	printf("vs compile status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetShaderInfoLog(vertexShaderID, sizeof(buf), NULL, buf);
	printf("vs log = [%s]\n", buf);
	return vertexShaderID;
}
GLuint createFragmentShader(const char * sourceCode)
{
	GLint status;
	GLuint fragmentShaderID=glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID,1,(const GLchar **)&sourceCode,NULL);
	glCompileShader(fragmentShaderID);
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &status);
	printf("fs compile status = %s\n", (status == GL_TRUE) ? "true" : "false");
	glGetShaderInfoLog(fragmentShaderID, sizeof(buf), NULL, buf);
	printf("fs log = [%s]\n", buf);
	return fragmentShaderID;
}
GLuint createShaderProgram(GLuint vertexShaderID, GLuint fragmentShaderID)
{
	GLuint fullShaderID=glCreateProgram();
	glAttachShader(fullShaderID,vertexShaderID);
	glAttachShader(fullShaderID,fragmentShaderID);
	glLinkProgram(fullShaderID);
	return fullShaderID;
}
void wholeShader(void)
{
	cout<<"\n\nOpenGL: Teapot with Various Shading Methods:\n\n"<<endl;
	
	char * vertexShaderCode;
	char * fragmentShaderCode;
	cout<<"Enter\n1 for Flat Shading\n2 for Gouraud Shading\n3 for Phong Shading: "<<endl;
	cin>>Method;
	while(Method==0);
	while(Method!=1 && Method!=2 && Method!=3)
	{
		cout<<"ERROR! Invalid Method"<<endl;
		cout<<"Enter either 1, 2 or 3 for Shader Selection: "<<endl;
		cin>>Method;
	}
	if (Method==1)
	{
		cout<<"Flat Shader Selected"<<endl;
		vertexShaderCode= readfile("flatVS.txt");
		fragmentShaderCode= readfile("flatFS.txt");
	}
	else if(Method==2)
	{
		cout<<"Gouraud Shader Selected"<<endl;
		vertexShaderCode= readfile("gourVS.txt");
		fragmentShaderCode= readfile("gourFS.txt");
	}
	else
	{
		cout<<"Phong Shader Selected"<<endl;
		vertexShaderCode= readfile("phongVS.txt");
		fragmentShaderCode= readfile("phongFS.txt");
	}
	
	GLuint vertexShaderID=createVertexShader(vertexShaderCode);
	GLuint fragmentShaderID=createFragmentShader(fragmentShaderCode);
	shaderID= createShaderProgram(vertexShaderID,fragmentShaderID);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glUseProgram(shaderID);
	GLuint vboHandles[2];
	glGenBuffers(1, vboHandles);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandles[0]);
	glBufferData(GL_ARRAY_BUFFER, 2 *num * 4 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER,0, num * 4 * sizeof(float), vertex);
	glBufferSubData(GL_ARRAY_BUFFER, num * 4 * sizeof(float),num * 4 * sizeof(float), normal);

	GLuint vaoHandle, loc=0;
	glGenVertexArrays( 1, &vaoHandle );
	glBindVertexArray(vaoHandle);

	loc = glGetAttribLocation(shaderID, "VertexPosition");
	glEnableVertexAttribArray(loc); 
	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0,(GLvoid*)(0));
	loc = glGetAttribLocation(shaderID, "VertexNormal");
	glEnableVertexAttribArray(loc); 
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0,(GLvoid*)(num*4*sizeof(GLfloat)));
	glEnable(GL_DEPTH_TEST);
}
void display(void) {
	GLuint temp;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setLK();
	setMatrices();
	temp = glGetUniformLocation(shaderID, "ModelViewMatrix");
	glUniformMatrix4fv(temp, 1, GL_FALSE, &ModelViewMatrix[0][0] );
	temp = glGetUniformLocation(shaderID, "NormalMatrix");
	glUniformMatrix4fv(temp, 1, GL_FALSE, &NormalMatrix[0][0]);
	temp = glGetUniformLocation(shaderID, "ProjectionMatrix");
	glUniformMatrix4fv(temp, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	temp = glGetUniformLocation(shaderID, "MVP");
	glUniformMatrix4fv(temp, 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, num);
	glutSwapBuffers();
}

int main (int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGB| GLUT_DEPTH);
	glutInitWindowSize(750,750);
	glutInitWindowPosition(750,50);
	glutCreateWindow("GL Project");

	const GLubyte *renderer = glGetString( GL_RENDERER );
	const GLubyte *vendor = glGetString( GL_VENDOR );
	const GLubyte *version = glGetString( GL_VERSION );
	const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("GL Vendor : %s\n", vendor);
	printf("GL Renderer : %s\n", renderer);
	printf("GL Version (string) : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version : %s\n", glslVersion);

	GLenum err = glewInit();

	generateSphere(4);
	wholeShader();

	glutDisplayFunc(display);

	glutMainLoop();
}
