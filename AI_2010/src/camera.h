#include <math.h>

class Camera{
public:
	
	float Yaw, Pitch, Roll;
	bool yaw_on, pitch_on, roll_on, jump_on;
	int speed;
	float veloy, gravity;

	//used for storing forward, movement and position vectors
	struct cam{
		float x, y, z;
	}forward, up, movement, position;

	Camera();
	void CameraRightVector();
	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	void MoveForward();
	void MoveBack();
	void Update(float roll, float pitch, float yaw);
	void Update();
	void OnlyPitch(float pitch);
	void OnlyPitch(float pitch, float limit);
	void OnlyRoll(float roll);
	void OnlyYaw(float yaw);
	void Jump();
	void CheckGround();
	void Stationary();

private:

};