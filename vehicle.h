#include <glm\glm.hpp>
#include <glm\ext.hpp> 
#include <math.h>

using namespace std;

class vehicle {
public:
	float x, y;
	float xCoord[4], yCoord[4];
	glm::vec3 direction;
	float speed;
	float acceleration, deceleration;
	float width, height;
	float weight;
	int idVehicle;
	int position;


	vehicle();
	vehicle(float, float, float, glm::vec3, float, float, float, float, float, int, int);

	float getAngle();
};

glm::vec3 normolize(glm::vec3);


vehicle::vehicle()
{
	x = 0.0, y = 0.0;
	speed = 0.0;
	direction = { 0.0, 0.0, 0.0 }; //glm::vec3(0.0, 0.0, 0.0)
	acceleration = 0.0, deceleration = 0.0;
	width = 3, height = 6;
	weight = 0.0;
	idVehicle = 0;
	position = 0;

	float w2 = width / 2;
	float h2 = height / 2;

	xCoord[0] = x - w2;
	yCoord[0] = y + h2;

	xCoord[1] = x + w2;
	yCoord[1] = y + h2;

	xCoord[2] = x + w2;
	yCoord[2] = y - h2;

	xCoord[3] = x - w2;
	yCoord[3] = y - h2;

}

vehicle::vehicle(float xx, float yy, float s, glm::vec3 des, float a, float d, float w, float h, float weig, int i, int p)
{
	x = xx, y = yy;
	speed = s;
	direction = normolize(des); //glm::vec3(0.0, 0.0, 0.0)
	acceleration = a, deceleration = d;
	width = w, height = h;
	weight = weig;
	idVehicle = i;
	position = p;

	float w2 = width / 2;
	float h2 = height / 2;

	xCoord[0] = x - w2;
	yCoord[0] = y + h2;

	xCoord[1] = x + w2;
	yCoord[1] = y + h2;

	xCoord[2] = x + w2;
	yCoord[2] = y - h2;

	xCoord[3] = x - w2;
	yCoord[3] = y - h2;
}

glm::vec3 normolize(glm::vec3 des)
{
	float lenght = sqrt(des.x * des.x + des.y * des.y + des.z * des.z);
	glm::vec3 res;
	res.x = des.x / lenght;
	res.y = des.y / lenght;
	res.z = des.z / lenght;
	return res;
}

float vehicle::getAngle()
{
	if (yCoord[0] == yCoord[1])
		return glm::angle(direction, glm::vec3(0.0, 1.0, 0.0));
	if (xCoord[0] == xCoord[1])
		return glm::angle(direction, glm::vec3(1.0, 0.0, 0.0));

	float f = glm::angle(direction, normolize(glm::vec3(((xCoord[0] + xCoord[1]) / 2) - x, ((yCoord[0] + yCoord[1]) / 2) - y, 0.0)));
	return f;
}
