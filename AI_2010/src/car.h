#ifndef _CAR_H_
#define _CAR_H_

class Car{
public:
	Car();
	~Car();
	void Init();
	void CleanUp();
	void Draw();
	//Inline Setters
	inline void SetVelocity(float v){velocity = v;}
	inline void SetX(float tempX){x = tempX;}
	inline void SetY(float tempY){y = tempY;}
	inline void SetZ(float tempZ){z = tempZ;}
	//Inline Getters
	inline float GetVelocity(){return velocity;}
	inline float GetX(){return x;}
	inline float GetY(){return y;}
	inline float GetZ(){return z;}
	//Inline Update Velocity
	inline void UpdateVelocity(float v){velocity += v;}
	void Move();
private:
	float x, y, z;
	float velocity;
};
#endif