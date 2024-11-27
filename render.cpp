#define GL_GLEXT_PROTOTYPES

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include "render.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"



GLuint vbo[2];		//identyfikatory buforow wierzcholkow
GLuint vao[1];		//identyfikatory tablic wierzcholkow
GLuint ebo;		//identyfikator bufora elementow

GLuint shaderProgram;
GLint vertexShader;	//identyfikator shadera wierzcholkow
GLint fragmentShader;   //identyfikator shadera fragmentow
GLint posAttrib, colAttrib;	//

glm::mat4 viewMatrix = glm::mat4();
glm::mat4 projectionMatrix = glm::mat4(); //marzerz widoku i rzutowania
GLfloat fi = 0;


//-------------Atrybuty wierzcholkow------------------------------------------
/*
	GLfloat ver_rectangle[] = {	//wspolrzedne wierzcholkow prostokata
		-1.0f, -0.5f, 0.5f, //0
		 1.0f, -0.5f, 0.5f, //1
		1.0f, 0.5f, 0.5f, //2
		 -1.0f, 0.5f, 0.5f, //3
    -1.0f, -0.5f, -0.5f, //4
    1.0f, -0.5f, -0.5f, //5
    1.0f, 0.5f, -0.5f, //6
    -1.0f, 0.5f, -0.5f //7
	};

	GLfloat col_rectangle[] = {	//kolory wierzcholkow prostokata
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f
	};

	GLuint elements[] = { //prostokat skladamy z dwoch trojkatow
		0, 1, 2,		  //indeksy wierzcholkow dla pierwszego trojkata
		0, 2, 3,	  //indeksy wierzcholkow dla drugiego trojkata
    0, 1, 5,
    0, 4, 5,
    4, 5, 6,
    4, 6, 7,
    2, 3, 6,
    3, 6, 7,
    1, 2, 5,
    2, 5, 6,
    0, 3, 4,
    3, 4, 7
	};
	
*/

  GLfloat ver_cone[] ={
    0.0f, 2.0f, 0.0f,
    -0.5f, 0.0f, 0.0f,
    -0.4f, 0.0f, -0.4f,
    0.0f, 0.0f, -0.5f,
    0.4f, 0.0f, -0.4f,
    0.5f, 0.0f, 0.0f,
    0.4f, 0.0f, 0.4f,
    0.0f, 0.0f, 0.5f,
    -0.4f, 0.0f, 0.4f
  };

  GLfloat col_cone[] = {
		1.0f, 1.0f, 1.0f,
		0.0f, 0.2f, 0.0f,
		0.0f, 0.4f, 0.0f,
		0.0f, 0.2f, 0.0f,
    0.0f, 0.4f, 0.0f,
    0.0f, 0.2f, 0.0f,
    0.0f, 0.4f, 0.0f,
    0.0f, 0.2f, 0.0f
	};

  GLuint elements[] = { 
		0, 1, 2,		 
		0, 2, 3,	  
    0, 3, 4,
    0, 4, 5,
    0, 5, 6,
    0, 6, 7,
    0, 7, 8
	};
//----------------------------kod shadera wierzcholkow-----------------------------------------

const GLchar* vShader_string =
{
  "#version 130\n"\

  "in vec3 position;\n"\
  "in vec3 color;\n"\
  "out vec3 Color;\n"\
  "uniform mat4 transformMatrix;\n"\
  "void main(void)\n"\
  "{\n"\
  "  gl_Position = transformMatrix * vec4(position, 1.0);\n"\
  "  Color = color;\n"\
  "}\n"
};

//----------------------------kod shadera fragmentow-------------------------------------------
const GLchar* fShader_string =
{
  "#version 130\n"\
  "in  vec3 Color;\n"\
  "out vec4 outColor;\n"\

  "void main(void)\n"\
  "{\n"\
  "  outColor = vec4(Color, 1.0);\n"\
  "}\n"
};


//------------------------------------------------zmiana rozmiaru okna---------------------------

void resizeGLScene(unsigned int width, unsigned int height)
{
    if (height == 0) height = 1; // zabezpiecznie dla okna o zerowej wysokosci
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 500.0f);
    glMatrixMode(GL_MODELVIEW);
}


//----------------------------------tworzenie, wczytanie, kompilacja shaderow-------------------------

int initShaders(void)
{
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShader_string, NULL);
    glCompileShader(vertexShader);
    
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE)
      std::cout << "Kompilacja shadera wierzcholkow powiodla sie!\n";
    else
    {
      std::cout << "Kompilacja shadera wierzcholkow NIE powiodla sie!\n";
      return 0;
     }
     
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShader_string, NULL); 
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE)
      std::cout << "Kompilacja shadera fragmentow powiodla sie!\n";
    else
    {
      std::cout << "Kompilacja shadera fragmentow NIE powiodla sie!\n";
      return 0;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    
    //glBindFragDataLocation(shaderProgram, 0, "outColor"); 

    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    return 1;
}



//--------------------------------------------funkcja inicjujaca-------------------------------------
int initGL(void)
{
   
    if(initShaders())
    {   

        glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); 
	glGenVertexArrays(1, vao); //przypisanie do vao identyfikatorow tablic
	glGenBuffers(2, vbo);	   //przypisanie do vbo identyfikatorow buforow
	glGenBuffers(1, &ebo);

	posAttrib = glGetAttribLocation(shaderProgram, "position"); //pobranie indeksu tablicy atrybutow wierzcholkow okreslajacych polozenie
        glEnableVertexAttribArray(posAttrib);
	colAttrib = glGetAttribLocation(shaderProgram, "color");    //pobranie indeksu tablicy atrybutow wierzcholkow okreslajacych kolor
        glEnableVertexAttribArray(colAttrib);
	
	glBindVertexArray(vao[0]);
  /*
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ver_rectangle), ver_rectangle, GL_STATIC_DRAW);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(col_rectangle), col_rectangle, GL_STATIC_DRAW);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colAttrib);

  */
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ver_cone), ver_cone, GL_STATIC_DRAW);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(col_cone), col_cone, GL_STATIC_DRAW);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colAttrib);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
        
	//macierz widoku (okresla polozenie kamery i kierunek w ktorym jest skierowana) 
	viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
	//macierz rzutowania perspektywicznego
	projectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 10.0f);		

 	return 1;
    }
    else
	return 0;
}

//------------------------------------------renderowanie sceny-------------------------------------

int drawGLScene(int counter)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    glm::mat4 translationMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));  		//macierz przesuniecia o zadany wektor
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(), glm::radians(fi), glm::vec3(0.0f, 1.0f, 0.0f)); //macierz obrotu o dany kat wokol wektora
		
    glm::mat4 transformMatrix = projectionMatrix * viewMatrix * translationMatrix * rotationMatrix; //wygenerowanie macierzy uwzgledniajacej wszystkie transformacje


    GLint transformMatrixUniformLocation = glGetUniformLocation(shaderProgram, "transformMatrix");  //pobranie polozenia macierzy bedacej zmienna jednorodna shadera
    glUniformMatrix4fv(transformMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix)); //zapisanie macierzy bedacej zmienna jednorodna shadera wierzcholkow
    

    glBindVertexArray(vao[0]);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); //rysowanie prostokata
    glDrawArrays(GL_TRIANGLE_FAN, 0, 10);

    fi += 0.5;
 
    glFlush();

    return 1;    
}

//----------------------------------------------------porzadki--------------------------------------

void deleteAll()
{
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(2, vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, vao);
}
