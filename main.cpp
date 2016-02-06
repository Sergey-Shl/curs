#include <gl/glew.h>
#include <gl/freeglut.h>
#include <iostream>

#include "loadshaders.h"
#include "vehicle.h"
#include "column.h"

using namespace std;

class cell
{
public:
	int type; // 0 - free space, 1 - road, 2 - intersection
	int dir[4];
};


GLuint program;
GLuint VertextBuffer;
GLuint ColorBuffer;
GLuint VA;
GLuint shaderprogram;

GLuint MatrixID;
glm::mat4 MVP;

const int max_veh = 10;
const int map_size = 20;

cell r = { 1, 0 };
cell fs = { 0, 0 };
cell intersec[5]; //intersections

cell map[map_size * map_size] = { fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, { 2, { 0, 0, 0, 0 } }, fs, fs, fs, fs, fs, fs, fs,
fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, fs, fs, fs, fs,
fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, fs, fs, fs, fs,
fs, fs, fs, fs, fs, fs, fs, fs, { 2, { 0, 1, 0, 0 } }, r, r, r, r, r, r, r, r, r, { 2, { 0, 0, 1, 0 } }, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, r, fs, fs, fs, fs, fs, r, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, r, fs, fs, fs, fs, fs, r, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, r, fs, fs, fs, fs, fs, r, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, r, fs, fs, fs, fs, fs, r, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, r, fs, fs, fs, fs, fs, r, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, r, fs, fs, fs, fs, fs, r, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, r, fs, fs, fs, fs, fs, r, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, r, fs, fs, fs, fs, fs, r, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, r, fs, fs, fs, fs, fs, r, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, { 2, { 1, 0, 0, 0 } }, r, r, r, r, r, { 2, { 0, 0, 0, 1 } }, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs,
fs, fs, fs, fs, fs, fs, fs, fs, r, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs,
fs, fs, fs, fs, fs, fs, fs, fs, { 2, { 1, 0, 0, 0 } }, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs, fs };

column c(max_veh);


glm::vec3 rotate(glm::vec3 cpoint, glm::vec3 point, float angle){
	glm::vec3 rotated_point;
	rotated_point.x = cpoint.x + (point.x - cpoint.x) * cos(angle) - (point.y - cpoint.y) * sin(angle);
	rotated_point.y = cpoint.y + (point.y - cpoint.y) * cos(angle) + (point.x - cpoint.x) * sin(angle);
	rotated_point.z = 0;
	return rotated_point;
}

void init(void)
{
	glGenVertexArrays(1, &VA);
	glBindVertexArray(VA);

	GLfloat colors[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.00f, 0.0f, 0.0f,
		1.00f, 0.0f, 0.0f
	};

	glGenBuffers(1, &VertextBuffer);
	glGenBuffers(1, &ColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	ShaderInfo shaders[] =
	{
		{ GL_VERTEX_SHADER, "NULL" },
		{ GL_FRAGMENT_SHADER, "NULL" },
		{ GL_NONE, NULL }
	};

	shaderprogram = LoadShaders(shaders);

	MatrixID = glGetUniformLocation(shaderprogram, "MVP");

	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 Scal = glm::scale(glm::vec3(1.0, 1.0, 1.0));
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 15), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	glm::mat4 Model = glm::mat4(1.0f);

	MVP = Scal * Projection * View * Model;
}

void display(void)
{
	glUseProgram(shaderprogram);
	glClear(GL_COLOR_BUFFER_BIT);

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VertextBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	for (int i = 0; i < c.getTop(); i++)
	{
		glDrawArrays(GL_TRIANGLES, i * 8, 6);
		glDrawArrays(GL_LINE_STRIP, ((i + 1) * 8) - 2, 2);
		if (i == c.getTop() - 1)
		{
			glDrawArrays(GL_LINES, ((i + 1) * 8), 80);
		}
	}



	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	//glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'A');

	glutSwapBuffers();
}

int frame = 0, t = 0, timebase = 0;
float fps;

void fpscalculate()
{
	frame++;
	t = glutGet(GLUT_ELAPSED_TIME);

	if (t - timebase > 1000) {
		fps = (frame*1000.0 / (t - timebase));
		timebase = t;
		frame = 0;
	}

	cout << "FPS: " << fps << endl;
}


float numticks = 0;

const int TICKS_PER_SECOND = 60;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 5;

unsigned long next_game_tick = glutGet(GLUT_ELAPSED_TIME);
unsigned long elapsed_time = 0;

int loops;
float interpolation;

void game()
{

}

void update_game(int = 1)
{
	if (glutGet(GLUT_ELAPSED_TIME) > next_game_tick && loops < MAX_FRAMESKIP) {
		game();
		next_game_tick += SKIP_TICKS;
		loops++;
		glutTimerFunc(SKIP_TICKS, update_game, 1);
	}
}

void move()
{
	for (int i = 0; i < c.getTop(); i++)
	{


		int x1 = (((c.Peek(i).x - 5) / 10) + 10);
		int y1 = 20 - (((c.Peek(i).y - 5) / 10) + 10);
		//map[y1 * map_size + x1].type = 3;

		if (map[y1 * map_size + x1].type == 2)
		{
			if (map[y1 * map_size + x1].dir[0] == 1)
			{
				c.Peek(i).direction.x = 0;
				c.Peek(i).direction.y = 1;
			} else if (map[y1 * map_size + x1].dir[1] == 1)
			{
				c.Peek(i).direction.x = 1;
				c.Peek(i).direction.y = 0;
			} else	if (map[y1 * map_size + x1].dir[2] == 1)
			{
				c.Peek(i).direction.x = 0;
				c.Peek(i).direction.y = -1;
			} else if (map[y1 * map_size + x1].dir[3] == 1)
			{
				c.Peek(i).direction.x = -1;
				c.Peek(i).direction.y = 0;
			}
			else 
			{
				c.Peek(i).speed = 0.0;
			}
		}

		/*
		if (c.Peek(i).x > 65)
		{
			c.Peek(i).direction.x = 0;
			c.Peek(i).direction.y = -1;
		}

		if (c.Peek(i).x > 65 && c.Peek(i).y < -25)
		{
			c.Peek(i).direction.x = -1;
			c.Peek(i).direction.y = 0;
		}

		if (c.Peek(i).x < -65 && c.Peek(i).y < -25)
		{
			c.Peek(i).direction.x = 0;
			c.Peek(i).direction.y = 1;
		}
		*/

		c.Peek(i).direction = normolize({ c.Peek(i).direction.x, c.Peek(i).direction.y, 0.0 });

		c.Peek(i).x += c.Peek(i).direction.x * c.Peek(i).speed;
		c.Peek(i).y += c.Peek(i).direction.y * c.Peek(i).speed;

		for (int j = 0; j < 4; j++)
		{
			c.Peek(i).xCoord[j] += c.Peek(i).direction.x * c.Peek(i).speed;
			c.Peek(i).yCoord[j] += c.Peek(i).direction.y * c.Peek(i).speed;
		}
		float angle = c.Peek(i).getAngle();
		c.Peek(i).rotate(angle);

	}
}

void timer(int = 0)
{
	int const numVertex = 8 * 3;
	GLfloat vertex[max_veh * numVertex + 40 * 3 * 2];

	move();

	for (int i = 0; i < c.getTop(); i++)
	{
		vehicle car = c.Peek(i);
		float w2 = car.width / 2;
		float h2 = car.height / 2;
		float x = car.x;
		float y = car.y;
		int j = 0;
		// Triangle 1
		vertex[(i * numVertex) + j++] = car.xCoord[0] ;
		vertex[(i * numVertex) + j++] = car.yCoord[0];
		vertex[(i * numVertex) + j++] = 0.0;
		vertex[(i * numVertex) + j++] = car.xCoord[3] ;
		vertex[(i * numVertex) + j++] = car.yCoord[3] ;
		vertex[(i * numVertex) + j++] = 0.0;
		vertex[(i * numVertex) + j++] = car.xCoord[2] ;
		vertex[(i * numVertex) + j++] = car.yCoord[2] ;
		vertex[(i * numVertex) + j++] = 0.0;
		// Triangle 2
		vertex[(i * numVertex) + j++] = car.xCoord[1] ;
		vertex[(i * numVertex) + j++] = car.yCoord[1];
		vertex[(i * numVertex) + j++] = 0.0;
		vertex[(i * numVertex) + j++] = car.xCoord[2] ;
		vertex[(i * numVertex) + j++] = car.yCoord[2] ;
		vertex[(i * numVertex) + j++] = 0.0;
		vertex[(i * numVertex) + j++] = car.xCoord[0] ;
		vertex[(i * numVertex) + j++] = car.yCoord[0] ;
		vertex[(i * numVertex) + j++] = 0.0;
		//Vector
		vertex[(i * numVertex) + j++] = (x ) + (car.direction.x) * 50;
		vertex[(i * numVertex) + j++] = (y ) + (car.direction.y) * 50;
		vertex[(i * numVertex) + j++] = 0.0;
		vertex[(i * numVertex) + j++] = (x);
		vertex[(i * numVertex) + j++] = (y);
		vertex[(i * numVertex) + j++] = 0.0;

		if (i == c.getTop() - 1)
		{
			int index = (i * numVertex) + j;
			/*vertex[index++] = -2;
			vertex[index++] = -50;
			vertex[index++] = 0;
			vertex[index++] = -2;
			vertex[index++] = 5;
			vertex[index++] = 0;
			vertex[index++] = 30;
			vertex[index++] = 5;
			vertex[index++] = 0;
			vertex[index++] = 30;
			vertex[index++] = -30;
			vertex[index++] = 0; */
			for (int k = 0; k < 20; k++)
			{
				vertex[index++] = -150;
				vertex[index++] = (k - 10) * 10;
				vertex[index++] = 0;
				vertex[index++] = 150;
				vertex[index++] = (k - 10) * 10;
				vertex[index++] = 0;

				vertex[index++] = (k - 10) * 10;
				vertex[index++] = -150;
				vertex[index++] = 0;
				vertex[index++] = (k - 10) * 10;
				vertex[index++] = 150;
				vertex[index++] = 0;
			}
		}


		j = 0;


	}

	/*
	system("cls");
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			if (map[i * map_size + j].type == 0)
				cout << " ";
			if (map[i * map_size + j].type == 1)
				cout << "1";
			if (map[i * map_size + j].type == 2)
				cout << "*";
			if (map[i * map_size + j].type == 3)
				cout << "C";
			if (j == 19)
				cout << "end" << endl;
		}
	}
	*/
	glBindBuffer(GL_ARRAY_BUFFER, VertextBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_DYNAMIC_DRAW);

	//fpscalculate();

	display();

	for (int i = 1; i < c.getTop(); i++)
	{
		//c.Peek(i).checkDis(c.Peek(i - 1));
	}

	


	glutTimerFunc(5, timer, 0);
}

void MouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		cout << x << " " << y << endl;
}

vehicle car1(5.0, 0.0, 0.1, glm::vec3(0.0, 1.0, 0.0), 0.001, 0.0009, 4.0, 9.0, 1500.0, 0, 0);
vehicle car2(5.0, -20, 0.2, glm::vec3(0.0, 1.0, 0.0), 0.001, 0.0009, 3.0, 6.0, 1500.0, 1, 1);
vehicle car3(5.0, -46, 0.3, glm::vec3(0.0, 1.0, 0.0), 0.001, 0.0009, 3.0, 6.0, 1500.0, 2, 2);
vehicle car4(-10.0, 0.0, 0.25, glm::vec3(0.0, 1.0, 0.0), 1.0, 0.99, 2.0, 5.0, 1500.0, 0, 0);
vehicle car5(-5.0, 0.0, 0.2, glm::vec3(0.0, 1.0, 0.0), 4.0, 0.0, 2.5, 4.0, 1500.0, 4, 4);

vehicle car;

int main(int argc, char** argv) {
	for (int i = 0; i < max_veh; i++)
	{
		car.x = (i - max_veh / 2) * 0 - 15;
		car.y = -100 - (i - max_veh / 2) * 15;
		//car.y = 0;
		car.speed = +0.20;

		car.direction = normolize({ 0.0, 1.0, 0.0 });
		car.acceleration = 0.01;
		car.deceleration = 0.0009;
		car.width = 3;
		car.height = 6;
		car.idVehicle = i;
		car.position = i;
		float w2 = car.width / 2;
		float h2 = car.height / 2;

		if (car.direction.y > 0)
		{

			car.xCoord[0] = car.x - w2;
			car.yCoord[0] = car.y + h2;

			car.xCoord[1] = car.x + w2;
			car.yCoord[1] = car.y + h2;

			car.xCoord[2] = car.x + w2 + 0.5;
			car.yCoord[2] = car.y - h2;

			car.xCoord[3] = car.x - w2 - 0.5;
			car.yCoord[3] = car.y - h2;
		}

		if (car.direction.y < 0)
		{

			car.xCoord[0] = car.x - w2;
			car.yCoord[0] = car.y - h2;

			car.xCoord[1] = car.x + w2;
			car.yCoord[1] = car.y - h2;

			car.xCoord[2] = car.x + w2 + 0.5;
			car.yCoord[2] = car.y + h2;

			car.xCoord[3] = car.x - w2 - 0.5;
			car.yCoord[3] = car.y + h2;
		}



		c.push(car);
	}

	//c.Peek(0).speed = 0.2;
	//c.printStack();



	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(728, 728);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(argv[0]);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW ... exiting" << endl;
		exit(EXIT_FAILURE);
	}
	init();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutDisplayFunc(display);
	glutMouseFunc(MouseFunc);
	//update_game();
	timer();
	glutMainLoop();
}
