#include "car.h"

Car::Car(){
	Init();
}
Car::~Car(){

}
//Initalise Car Variables
void Car::Init(){
	x = 0.0f;
	y = -14.0f;
	z = 5.0f;
	velocity = 0.0f;
}
void Car::CleanUp(){

}
//Update the X Position based upon velocity
void Car::Move(){
	x += velocity;
}