#ifndef _FUZZY_H_
#define _FUZZY_H_

namespace fl{
	class Engine;
	class InputVariable;
	class OutputVariable;
}

class Fuzzy{
public:
	Fuzzy();
	~Fuzzy();
	void Init();
	void EngineInit();
	void InputVariablesInit();
	void OutputVariableInit();
	void RuleBlockInit();
	void CleanUp();
	//Calculate the change needed from 
	float CalculateVelocityDelta(float position, float velocity);
private:
	/*Create Variables for FuzzyLite Libarary 
	found here http://www.fuzzylite.com/ */
	fl::Engine* engine;
	fl::OutputVariable* outputVariable;
	fl::InputVariable* inputVariable1;
	fl::InputVariable* inputVariable2;
};
#endif