#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "tex.h"
#include "camera.h"                                    
#include "fuzzy.h"
#include "car.h"
#define COLOUR_DEPTH 16	//Colour depth

typedef struct Mouse
{
	int x,y;
}Mouse;
struct Textxy{
	float x, y;
};
struct Vector3{
	float x, y, z;
};
struct Data{
	bool negative;
	int  integer;
	int first;
	int second;
	int third;
	int fourth;
	int fifth;
	int sixth;
};
//Global
HWND        ghwnd;
HDC			ghdc;
HGLRC		ghrc;			//hardware RENDERING CONTEXT
HINSTANCE	ghInstance;
RECT		gRect;
RECT		screenRect;
int S_WIDTH	 =	800;		//client area resolution
int S_HEIGHT =	600;
bool		keys[256];
//Textures for objects 
GLuint		GroundTexture;
GLuint		CarTexture;
GLuint		LineTexture;
//Textures for Text
GLuint		CarPositionTexture;
GLuint		CarVelocityTexture;
GLuint		LinePositionTexture;
GLuint		DistanceTexture;
GLuint		ResultTexture;
//Textures for symbols
GLuint		DotTexture;
GLuint		MinusTexture;
GLuint		StarTexture;
GLuint		BlankTexture;
//Textures for numbers
GLuint		NumberZeroTexture;
GLuint		NumberOneTexture;
GLuint		NumberTwoTexture;
GLuint		NumberThreeTexture;
GLuint		NumberFourTexture;
GLuint		NumberFiveTexture;
GLuint		NumberSixTexture;
GLuint		NumberSevenTexture;
GLuint		NumberEightTexture;
GLuint		NumberNineTexture;
//Camera and mouse variables
GLuint		Dlist;
Mouse		MousePos;
Camera		camera;
//Texture Points
Textxy		textpts[4];
//Variables that store the integer and decimal parts of variables used
Data		carPositionData;
Data		carVelocityData;
Data		resultData;
Data		linePositionData;
Data		distanceData;
Data		editData;
//Vector for the line
Vector3 linePosition;
//paused switch
bool paused = false;
//position of the star in menu
int starPosition = 0;
//switch for if the values are being edited
bool editPaused = false;
//stores string for edited value
char editValueString[20];
//switch for edited value being negative
bool editNegative = false;
//switch for is the dot has already been pressed
bool editDot = false;
//different ambient settings for the lights
GLfloat Light_Ambient_1[] = {1.75f, 1.75f, 1.75f, 1.0f};
//different diffuse settings for the lights
GLfloat Light_Diffuse_1[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat Light_Position[]= {0.0f, 0.0f, 0.0f, 1.0f};
//Fuzzy Inference System
Fuzzy carFuzzy;
//Car Object
Car myCar;
//Result from fuzzy system
float fuzzyResult;

//Function Prototypes
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
HWND CreateOurWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance);
void ResizeGLWindow(int width, int height);
void Init(HWND hwnd);
//Initialise Textures
void InitTextures();
//Functions for the logic of the application
void Logic();
void MovementLogic();
//Keypress Variables
void MoveLine(float delta);
void AddNumbers();
void StoreValues();
//Update edit variable
void ClearEdit();
void UpdateVariable();
float CheckVariableLimit();
//Draw the Scene
void DrawScene();
void DrawGround();
void DrawCar();
void DrawLine();
//Drae Pause Menu
void DrawPause();
void DrawMenu();
void DrawValues(Data data, float z);
void DrawPauseBlocks(float x, float z, float width);
//Update Data variables
void ClearData(Data& data);
void UpdateData();
//Calculate what numbers to show.
void ChooseNumbers(int value);
Data CalculateTextures(float value);

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message){														
		case WM_CREATE:	
			break;
		case WM_SIZE:
			//resize the open gl window when the window is resized
			ResizeGLWindow(LOWORD(lParam),HIWORD(lParam));
			GetClientRect(hwnd, &screenRect);
			break;
		case WM_KEYDOWN:
			keys[wParam]=true;
			break;
		case WM_KEYUP:
			keys[wParam]=false;
			break;
		case WM_MOUSEMOVE:
			MousePos.x = LOWORD (lParam);
			MousePos.y = HIWORD (lParam);
			break;
		case WM_PAINT:
			break;		

		case WM_DESTROY:	
			PostQuitMessage(0);					
			break;				
	}													
	return DefWindowProc (hwnd, message, wParam, lParam);														
}
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PSTR szCmdLine, int nCmdShow)			
{	
	HWND		hwnd;
	MSG         msg;		
	//texture points for the texture animation
	textpts[0].x = 0.0f;
	textpts[0].y = 0.0f;
	textpts[1].x = 0.0f;
	textpts[1].y = 0.5f;
	textpts[2].x = 1.0f;
	textpts[2].y = 0.5f;
	textpts[3].x = 1.0f;
	textpts[3].y = 0.0f;
	//Clear Data Variables
	ClearData(carPositionData);
	ClearData(distanceData);
	ClearData(linePositionData);
	ClearData(carVelocityData);
	ClearData(resultData);
	//Initialise objects
	linePosition.x = 0;
	myCar.Init();
	carFuzzy.Init();
	float position = linePosition.x-myCar.GetX();
	//caluclate the return value from the Fuzzy Inference System
	fuzzyResult = carFuzzy.CalculateVelocityDelta(position, myCar.GetVelocity()*100);
	//initialise and create window
	hwnd = CreateOurWindow("Graphics Scene", S_WIDTH, S_HEIGHT, 0, false, hInstance);	
	if(hwnd == NULL) return true;
	//initialise opengl and other settings
	Init(hwnd);
	while (true){							
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
			if (msg.message==WM_QUIT)
				break;
			TranslateMessage (&msg);							
			DispatchMessage (&msg);
		}
		else if(paused == false){
			//if not paused draw scene and do logic for moving objects
			MovementLogic();
			DrawScene();
		}else{
			//if paused do editing logic and draw the pause menu
			Logic();
			DrawPause();
		}
	}
	return msg.wParam ;										
}
HWND CreateOurWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance)
{
	HWND hwnd;
	WNDCLASS wcex;

	memset(&wcex, 0, sizeof(WNDCLASS));
	wcex.style			= CS_HREDRAW | CS_VREDRAW;		
	wcex.lpfnWndProc	= WndProc;		
	wcex.hInstance		= hInstance;						
	wcex.hIcon			= LoadIcon(NULL, IDI_APPLICATION);; 
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);		
	wcex.hbrBackground	= (HBRUSH) (COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;	
	wcex.lpszClassName	= "FirstWindowClass";	

	RegisterClass(&wcex);// Register the class

	dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	ghInstance	= hInstance;// Assign our global hInstance to the window's hInstance

	//Set the Client area of the window to be our resolution.
	RECT glwindow;
	glwindow.left		= 0;		
	glwindow.right		= width;	
	glwindow.top		= 0;		
	glwindow.bottom		= height;	

	AdjustWindowRect( &glwindow, dwStyle, false);

	//Create the window
	hwnd = CreateWindow(	"FirstWindowClass", 
							strWindowName, 
							dwStyle, 
							0, 
							0,
							glwindow.right  - glwindow.left,
							glwindow.bottom - glwindow.top, 
							NULL,
							NULL,
							hInstance,
							NULL
							);

	if(!hwnd) return NULL;// If we could get a handle, return NULL

	ShowWindow(hwnd, SW_SHOWNORMAL);	
	UpdateWindow(hwnd);					
	SetFocus(hwnd);						

	return hwnd;
}
bool SetPixelFormat(HDC hdc) 
{ 
	PIXELFORMATDESCRIPTOR pfd = {0}; 
	int pixelformat; 
 
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);	// Set the size of the structure
	pfd.nVersion = 1;							// Always set this to 1
	// Pass in the appropriate OpenGL flags
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
	pfd.dwLayerMask = PFD_MAIN_PLANE;			// standard mask (this is ignored anyway)
	pfd.iPixelType = PFD_TYPE_RGBA;				// RGB and Alpha pixel type
	pfd.cColorBits = COLOUR_DEPTH;				// Here we use our #define for the color bits
	pfd.cDepthBits = COLOUR_DEPTH;				// Ignored for RBA
	pfd.cAccumBits = 0;							// nothing for accumulation
	pfd.cStencilBits = 0;						// nothing for stencil
 
	//Gets a best match on the pixel format as passed in from device
	if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == false ) { 
		MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
		return false; 
	} 
 
	//sets the pixel format if its ok. 
	if (SetPixelFormat(hdc, pixelformat, &pfd) == false) { 
		MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
		return false; 
	} 
 
	return true;
}
void ResizeGLWindow(int width, int height)// Initialize The GL Window
{
	if (height==0)// Prevent A Divide By Zero error
	{
		height=1;// Make the Height Equal One
	}
	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//calculate aspect ratio
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, 0.1 ,350.0f);

	glMatrixMode(GL_MODELVIEW);// Select The Modelview Matrix
	glLoadIdentity();// Reset The Modelview Matrix
}
void InitializeOpenGL(int width, int height) 
{  
	ghdc = GetDC(ghwnd);//  sets  global HDC

	if (!SetPixelFormat(ghdc))//  sets  pixel format
		PostQuitMessage (0);

	ghrc = wglCreateContext(ghdc);	//  creates  rendering context from  hdc
	wglMakeCurrent(ghdc, ghrc);		//	Use this HRC.
	ResizeGLWindow(width, height);	// Setup the Screen
}
void Init(HWND hwnd){
	ghwnd = hwnd;
	GetClientRect(ghwnd, &gRect);	//get rect into our handy global rect
	InitializeOpenGL(gRect.right, gRect.bottom);// initialise openGL
	//Initalise Textures
	InitTextures();
	//OpenGL settings
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.5f, 0.0f, 1.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Also, do any other setting ov variables here for your app if you wish. 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);// Linear Filtering
}
//Initialise Textures
void InitTextures(){
	//create all textures required
	CreateGLTexture("ground.tga", GroundTexture);
	CreateGLTexture("car.tga", CarTexture);
	CreateGLTexture("blank.tga", BlankTexture);
	CreateGLTexture("line.tga", LineTexture);

	CreateGLTexture("carpos.tga", CarPositionTexture);
	CreateGLTexture("carvelo.tga", CarVelocityTexture);
	CreateGLTexture("linepos.tga", LinePositionTexture);
	CreateGLTexture("distance.tga", DistanceTexture);
	CreateGLTexture("result.tga", ResultTexture);

	CreateGLTexture("star.tga", StarTexture);
	CreateGLTexture("minus.tga", MinusTexture);
	CreateGLTexture("dot.tga", DotTexture);

	CreateGLTexture("zero.tga", NumberZeroTexture);
	CreateGLTexture("one.tga", NumberOneTexture);
	CreateGLTexture("two.tga", NumberTwoTexture);
	CreateGLTexture("three.tga", NumberThreeTexture);
	CreateGLTexture("four.tga", NumberFourTexture);
	CreateGLTexture("five.tga", NumberFiveTexture);
	CreateGLTexture("six.tga", NumberSixTexture);
	CreateGLTexture("seven.tga", NumberSevenTexture);
	CreateGLTexture("eight.tga", NumberEightTexture);
	CreateGLTexture("nine.tga", NumberNineTexture);
}
//Functions for the logic of the application
void Logic(){
	if (keys['Q']){
		//Unpause application
		paused = false;
		keys['Q'] = false;
	}else if(keys['W']){
		//Move the star up to choose which value to edit
		if(starPosition > 0 ){
			starPosition--;
		}
		//change edit variables
		editPaused = false;
		editDot = false;
		editNegative = false;
		memset(editValueString, 0x00 , sizeof(editValueString));
		keys['W'] = false;
	}else if(keys['S']){
		//move the star down
		if(starPosition < 2 ){
			starPosition++;
		}
		//change edit variables
		editPaused = false;
		editDot = false;
		editNegative = false;
		memset(editValueString, 0x00 , sizeof(editValueString));
		keys['S'] = false;
	}else if(keys[VK_BACK]){
		//turn edit on and off
		editPaused = !editPaused;
		editDot = false;
		memset(editValueString, 0x00 , sizeof(editValueString));
		keys[VK_BACK] = false;
	}else if(keys[VK_RETURN]){
		if(editPaused == true){
			//store the string
			StoreValues();
		}
		keys[VK_RETURN] = false;
	}else if(keys[VK_ADD]){
		if(editPaused == true){
			//set negative to be false
			editNegative = false;
			UpdateVariable();
		}
		keys[VK_ADD] = false;
	}else if(keys[VK_SUBTRACT]){
		if(editPaused == true){
			//set negative to be true
			editNegative = true;
			UpdateVariable();
		}
		keys[VK_SUBTRACT] = false;
	}
	if(editPaused == true){
		//add numbers to the string
		AddNumbers();
	}
}
void MovementLogic(){
	//if q is pressed pause the application
	if (keys['Q']){
		paused = true;
		keys['Q'] = false;
		UpdateData();
	}else if(keys['D']){
		//move right
		MoveLine(0.01f);
	}else if(keys['A']){
		//move left
		MoveLine(-0.01f);
	}
	//calaculate the change need to the velocity
	float position = linePosition.x-myCar.GetX();
	fuzzyResult = carFuzzy.CalculateVelocityDelta(position, myCar.GetVelocity()*100);
	//update the velcoity based upon the Fuzzy Inference System
	myCar.UpdateVelocity(fuzzyResult/100);
	//move the car
	myCar.Move();
	//update the camera based upon mouse movement
	camera.Update(0,-90,0);
	//move the texture points along
	for(int i = 0; i < 4; i++){
		textpts[i].y -= 0.0001f;
	}
}
//Keypress Functions
void MoveLine(float delta){
	//move line across the screen
	linePosition.x += delta;
	if(linePosition.x > 5){
		linePosition.x = 5;
	}else if(linePosition.x < -5){
		linePosition.x = -5;
	}
}
void AddNumbers(){
	//add key pressed to the string
	if(keys['0']){
		strcat(editValueString,"0");
		keys['0'] = false;
		UpdateVariable();
	}else if(keys['1']){
		strcat(editValueString,"1");
		keys['1'] = false;
		UpdateVariable();
	}else if(keys['2']){
		strcat(editValueString,"2");
		keys['2'] = false;
		UpdateVariable();
	}else if(keys['3']){
		strcat(editValueString,"3");
		keys['3'] = false;
		UpdateVariable();
	}else if(keys['4']){
		strcat(editValueString,"4");
		keys['4'] = false;
		UpdateVariable();
	}else if(keys['5']){
		strcat(editValueString,"5");
		keys['5'] = false;
		UpdateVariable();
	}else if(keys['6']){
		strcat(editValueString,"6");
		keys['6'] = false;
		UpdateVariable();
	}else if(keys['7']){
		strcat(editValueString,"7");
		keys['7'] = false;
		UpdateVariable();
	}else if(keys['8']){
		strcat(editValueString,"8");
		keys['8'] = false;
		UpdateVariable();
	}else if(keys['9']){
		strcat(editValueString,"9");
		keys['9'] = false;
		UpdateVariable();
	}else if(keys[VK_OEM_PERIOD]){
		if(editDot == false){
			strcat(editValueString, ".");
			editDot = true;
		}
		keys[VK_OEM_PERIOD] = false;
		UpdateVariable();
	}
}
void StoreValues(){
	//Used to store the edit data into the corret variable
	editPaused = !editPaused;
	editDot = false;
	//make sure the variable is within the limits
	float newValue = CheckVariableLimit();		
	//a switch for setting the currently edited value to the correct location
	switch(starPosition){
	case 0: myCar.SetX(newValue); break;
	case 1: linePosition.x = newValue; break;
	case 2: myCar.SetVelocity(newValue/100); break;
	}
	//clear the edit data variable
	ClearData(editData);
	memset(editValueString, 0x00 , sizeof(editValueString));
	editNegative = false;
	//update value from Fuzzy Inference System
	float position = linePosition.x-myCar.GetX();
	fuzzyResult = carFuzzy.CalculateVelocityDelta(position, myCar.GetVelocity()*100);
}
//Update edit variable
void ClearEdit(){
	//clear the variables that are being used for editing
	editDot = false;
	editNegative = false;
	memset(editValueString, 0x00 , sizeof(editValueString));
}
void UpdateVariable(){
	//convert string into float and update edit data variable
	float newValue = 0.0f;
	newValue = atof(editValueString);
	if(editNegative ==true){
		newValue *= -1;
	}
	editData = CalculateTextures(newValue);
}
float CheckVariableLimit(){
	//make sure the value is inside the limits for the Fuzzy Inference System
	float newValue = atof(editValueString);
	//if car position or line position set value to be 5
	//if velocity set to one
	if((starPosition == 0 || starPosition == 1 ) && newValue > 5){
		newValue = 5;
	}else if(starPosition == 2 && newValue > 1){
		newValue = 1;
	}
	//if negative change value to be negative
	if(editNegative ==true){
		newValue *= -1;
	}
	return newValue;
}
//Draw the Scene
void DrawScene(){	
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0f, 0.5f, 0.0f, 1.0f);				// Green Background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Clear The Screen And The Depth Buffer
	glLoadIdentity();// load Identity Matrix
	gluLookAt(camera.position.x, camera.position.y, camera.position.z, camera.forward.x, camera.forward.y, camera.forward.z, camera.up.x, camera.up.y, camera.up.z); //Where we are, What we look at, and which way is up
	
	//enable ambient, diffuse, and position of light 2
	glLightfv(GL_LIGHT2, GL_AMBIENT,  Light_Ambient_1);
	glLightfv(GL_LIGHT2, GL_DIFFUSE,  Light_Diffuse_1);
	glLightfv(GL_LIGHT2, GL_POSITION, Light_Position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT2);
	
	//draw the majority of the scene
	glPushMatrix();
		glPushMatrix();
			DrawLine();
		glPopMatrix();
		DrawCar();
		DrawGround();
	glPopMatrix();

	SwapBuffers(ghdc);// Swap the frame buffers.
}
void DrawGround(){
	//draw the ground on screen
	float width = 5.0f;
	float height = 7.0f;
	//draw the ground lower than the camera
	glTranslatef(0.0f, -15.0f, 5.0f);
	glBindTexture(GL_TEXTURE_2D,GroundTexture);
	glBegin (GL_QUADS);//Begin drawing state
		glColor3f(0.05f,0.05f,0.2f);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(textpts[0].x, textpts[0].y);
		glVertex3f(-width,0.0f,-height);

		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(textpts[1].x, textpts[1].y);
		glVertex3f(-width,0.0f,height);

		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(textpts[2].x, textpts[2].y);
		glVertex3f(width,0.0f,height);

		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(textpts[3].x, textpts[3].y);
		glVertex3f(width,0.0f,-height);
	glEnd();//end drawing
}
void DrawCar(){
	//draw car to screen
	glPushMatrix();
		glTranslatef(myCar.GetX()-0.5, myCar.GetY(), myCar.GetZ());
		glBindTexture(GL_TEXTURE_2D, CarTexture);
		glBegin(GL_QUADS);//Begin drawing state
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);

		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);

		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(1.0f, 0.0f, 1.0f);

		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(0.0f, 0.0f, 1.0f);
		glEnd();//end drawing
	glPopMatrix();
}
void DrawLine(){
	//draw line on screen
	glTranslatef(linePosition.x, linePosition.y-14.5f, linePosition.z+5.0f);
	glBindTexture(GL_TEXTURE_2D,LineTexture);
	glBegin (GL_QUADS);//Begin drawing state
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0f,0.0f,-7.0f);

		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.0f,0.0f,7.0f);

		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(0.1f,0.0f,7.0f);

		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(0.1f,0.0f,-7.0f);
	glEnd();//end drawing
}
//Drae Pause Menu
void DrawPause(){
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Clear The Screen And The Depth Buffer
	glLoadIdentity();// load Identity Matrix
	gluLookAt(camera.position.x, camera.position.y, camera.position.z, camera.forward.x, camera.forward.y, camera.forward.z, camera.up.x, camera.up.y, camera.up.z); //Where we are, What we look at, and which way is up
	
	//enable ambient, diffuse, and position of light 2
	glLightfv(GL_LIGHT2, GL_AMBIENT,  Light_Ambient_1);
	glLightfv(GL_LIGHT2, GL_DIFFUSE,  Light_Diffuse_1);
	glLightfv(GL_LIGHT2, GL_POSITION, Light_Position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT2);
	
	//draw the majority of the scene
	glPushMatrix();
		glTranslatef(0, -15.0f, 5.0f);
		DrawMenu();
	glPopMatrix();

	SwapBuffers(ghdc);// Swap the frame buffers.
}
void DrawMenu(){
	int star = 0;
	//Update the data vartiables
	UpdateData();
	//set the position of the star in the pause menu
	switch(starPosition){
	case 0: star = -5; break;
	case 1: star = -4; break;
	case 2: star = -2; break;
	}
	//draw star to screen
	glBindTexture(GL_TEXTURE_2D, StarTexture);
	DrawPauseBlocks(-8.5,star,1);
	//render car position text
	glBindTexture(GL_TEXTURE_2D, CarPositionTexture);
	DrawPauseBlocks(-7.5,-5,4.5);
	//draw line position text
	glBindTexture(GL_TEXTURE_2D, LinePositionTexture);
	DrawPauseBlocks(-7.5,-4,4.5);
	//draw distance text
	glBindTexture(GL_TEXTURE_2D, DistanceTexture);
	DrawPauseBlocks(-7.5,-3,4.5);
	//render car velocity text to the screen
	glBindTexture(GL_TEXTURE_2D, CarVelocityTexture);
	DrawPauseBlocks(-7.5,-2,4.5);
	//draw the result text
	glBindTexture(GL_TEXTURE_2D, ResultTexture);
	DrawPauseBlocks(-7.5,-1,4.5);
	//depending on star position display edit data variables
	//and then the remaing data values
	if(starPosition == 0 && editPaused == true){
		DrawValues(editData, -5);
		DrawValues(linePositionData, -4);
		DrawValues(carVelocityData, -2);
	}else if(starPosition == 1 && editPaused == true){
		DrawValues(carPositionData, -5);
		DrawValues(editData, -4);
		DrawValues(carVelocityData, -2);
	}else if(starPosition == 2 && editPaused == true ){
		DrawValues(carPositionData, -5);
		DrawValues(linePositionData, -4);
		DrawValues(editData, -2);
	}else{
		DrawValues(carPositionData, -5);
		DrawValues(linePositionData, -4);
		DrawValues(carVelocityData, -2);
	}
	//draw distance and result data
	DrawValues(distanceData, -3);
	DrawValues(resultData, -1);
}
void DrawValues(Data data, float z){
	//count is the position  on the secreen for each line of text
	int count = -3;
	//if negative draw minus sign else draw a blank square
	if(data.negative == true){
		glBindTexture(GL_TEXTURE_2D, MinusTexture);
	}else{
		glBindTexture(GL_TEXTURE_2D, BlankTexture);
	}
	DrawPauseBlocks(count++, z, 1);
	//if 10 or -10 then draw a 1 and 0 else draw a blank square and a corresponding number
	if(data.integer == 10 || data.integer ==-10){
		glBindTexture(GL_TEXTURE_2D, NumberOneTexture);
		DrawPauseBlocks(count++, z, 1);
		glBindTexture(GL_TEXTURE_2D, NumberZeroTexture);
		DrawPauseBlocks(count++, z, 1);
	}else{
		glBindTexture(GL_TEXTURE_2D, BlankTexture);
		DrawPauseBlocks(count++, z, 1);
		ChooseNumbers(data.integer);
		DrawPauseBlocks(count++, z, 1);
	}
	glBindTexture(GL_TEXTURE_2D, DotTexture);
	DrawPauseBlocks(count++, z, 1);
	//draw the corresponding decimal places with the correct values
	ChooseNumbers(data.first);
	DrawPauseBlocks(count++, z, 1);
	ChooseNumbers(data.second);
	DrawPauseBlocks(count++, z, 1);
	ChooseNumbers(data.third);
	DrawPauseBlocks(count++, z, 1);
	ChooseNumbers(data.fourth);
	DrawPauseBlocks(count++, z, 1);
	ChooseNumbers(data.fifth);
	DrawPauseBlocks(count++, z, 1);
	ChooseNumbers(data.sixth);
	DrawPauseBlocks(count++, z, 1);
}
void DrawPauseBlocks(float x, float z, float width){
	//draw a block at a select location
	glPushMatrix();
		glTranslatef(x, 0.0f, z);
		glBegin(GL_QUADS);//Begin drawing state
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);

		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0, 0.0f, 1.0f);

		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(width, 0.0f, 1.0f);

		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(width, 0.0f, 0.0f);
		glEnd();//end drawing
	glPopMatrix();
}
//Update Data variables
void ClearData(Data& data){
	//reset all data
	data.first = 0;
	data.integer = 0;
	data.negative = false;
	data.second = 0;
	data.third = 0;
	data.fourth = 0;
	data.fifth = 0;
	data.sixth = 0;
}
void UpdateData(){
	//update the data variables for displaying the text to the screen
	carPositionData = CalculateTextures(myCar.GetX());
	carVelocityData = CalculateTextures(myCar.GetVelocity()*100);
	linePositionData = CalculateTextures(linePosition.x);
	distanceData = CalculateTextures(linePosition.x - myCar.GetX());
	resultData = CalculateTextures(fuzzyResult);
}
//Calculate what numbers to show.
void ChooseNumbers(int value){
	//Swithc to determine which texture to use
	//use star texture if the value isn't below
	switch(value){
	case 0:glBindTexture(GL_TEXTURE_2D, NumberZeroTexture);break;
	case 1:glBindTexture(GL_TEXTURE_2D, NumberOneTexture);break;
	case 2:glBindTexture(GL_TEXTURE_2D, NumberTwoTexture);break;
	case 3:glBindTexture(GL_TEXTURE_2D, NumberThreeTexture);break;
	case 4:glBindTexture(GL_TEXTURE_2D, NumberFourTexture);break;
	case 5:glBindTexture(GL_TEXTURE_2D, NumberFiveTexture);break;
	case 6:glBindTexture(GL_TEXTURE_2D, NumberSixTexture);break;
	case 7:glBindTexture(GL_TEXTURE_2D, NumberSevenTexture);break;
	case 8:glBindTexture(GL_TEXTURE_2D, NumberEightTexture);break;
	case 9:glBindTexture(GL_TEXTURE_2D, NumberNineTexture);break;
	default: glBindTexture(GL_TEXTURE_2D, StarTexture);break;
	}
}
Data CalculateTextures(float value){
	//Used to fill data variables which will later be used later to display text to screen.
	Data tempValue;
	ClearData(tempValue);
	int tempNum;
	//Check to see if the value is negative
	if(value < 0){
		tempValue.negative = true;
		value *= -1;
	}else{
		tempValue.negative = false;
	}
	//Convert the value into its seperate parts.
	//Interger value is stored
	tempValue.integer = (int)value;
	//Store the first decimal place
	tempNum = ((int)(value*1000000)%1000000);
	tempValue.first = (int)(tempNum/100000);
	//Store the second decimal place
	tempNum -= tempValue.first*100000;
	tempValue.second = (int)(tempNum/10000);
	//Store the third decimal place
	tempNum -= tempValue.second*10000;	
	tempValue.third = (int)(tempNum/1000);
	//Store the fourth decimal place
	tempNum -= tempValue.third*1000;	
	tempValue.fourth = (int)(tempNum/100);
	//Store the fifth decimal place
	tempNum -= tempValue.fourth*100;	
	tempValue.fifth = (int)(tempNum/10);
	//Store the sixth decimal place
	tempNum -= tempValue.fifth*10;	
	tempValue.sixth = (int)(tempNum);
	return tempValue;
}
//Clean Up application
void Cleanup(){
	if (ghrc){
		wglMakeCurrent(NULL, NULL);	// free rendering memory
		wglDeleteContext(ghrc);		// Delete our OpenGL Rendering Context
	}

	if (ghdc) 
	ReleaseDC(ghwnd, ghdc);			// Release our HDC from memory

	UnregisterClass("FirstWindowClass", ghInstance);// Free the window class

	PostQuitMessage (0);		// Post a QUIT message to the window
}