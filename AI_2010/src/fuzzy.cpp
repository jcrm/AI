#include "fuzzy.h"
#include "fl/Engine.h"
#include "fl/defuzzifier/Centroid.h"
#include "fl/hedge/Any.h"
#include "fl/hedge/Extremely.h"
#include "fl/hedge/Not.h"
#include "fl/hedge/Seldom.h"
#include "fl/hedge/Somewhat.h"
#include "fl/hedge/Very.h"
#include "fl/rule/Rule.h"
#include "fl/rule/RuleBlock.h"
#include "fl/norm/s/Maximum.h"
#include "fl/norm/t/Minimum.h"
#include "fl/term/Accumulated.h"
#include "fl/term/Trapezoid.h"
#include "fl/term/Triangle.h"
#include "fl/term/Gaussian.h"
#include "fl/variable/InputVariable.h"
#include "fl/variable/OutputVariable.h"
#include "fl/norm/Norm.h"

Fuzzy::Fuzzy(){
}
void Fuzzy::InputVariablesInit(){
	//Distance from Line Input
	inputVariable1 = new fl::InputVariable;
	inputVariable1->setEnabled(true);
	inputVariable1->setName("LineOn");
	//Set range for distance from line as between -10 and 10.
	inputVariable1->setRange(-10.000, 10.000);
	//Set Membership Functions for distance from line (line - car position);
	inputVariable1->addTerm(new fl::Gaussian("VeryLeft", -10.000, 2.123));
	inputVariable1->addTerm(new fl::Gaussian("Left", -5.000, 2.123));
	inputVariable1->addTerm(new fl::Gaussian("Zero", 0.000, 2.123));
	inputVariable1->addTerm(new fl::Gaussian("Right", 5.000, 2.123));
	inputVariable1->addTerm(new fl::Gaussian("VeryRight", 10.000, 2.123));
	engine->addInputVariable(inputVariable1);
	//Velcoity Input
	inputVariable2 = new fl::InputVariable;
	inputVariable2->setEnabled(true);
	inputVariable2->setName("Steering");
	//SEt range between -1 and 1;
	inputVariable2->setRange(-1.000, 1.000);
	//Set Membership Functions for Steering(velcoity) 
	inputVariable2->addTerm(new fl::Gaussian("VeryLeft", -1.000, 0.212));
	inputVariable2->addTerm(new fl::Gaussian("Left", -0.500, 0.212));
	inputVariable2->addTerm(new fl::Gaussian("Zero", 0.000, 0.212));
	inputVariable2->addTerm(new fl::Gaussian("Right", 0.500, 0.212));
	inputVariable2->addTerm(new fl::Gaussian("VeryRight", 1.000, 0.212));
	engine->addInputVariable(inputVariable2);
}
void Fuzzy::OutputVariableInit(){
	outputVariable = new fl::OutputVariable;
	outputVariable->setEnabled(true);
	outputVariable->setName("Direction");
	//Set the Range to between -1 and 1;
	outputVariable->setRange(-1.000, 1.000);
	outputVariable->fuzzyOutput()->setAccumulation(new fl::Maximum);
	/*
	Set Defuzzification to use Centroid method.
	This is calculated from summing all moments of the 
	individual areas and dividing by the total area under the curve.
	*/
	outputVariable->setDefuzzifier(new fl::Centroid(200));
	outputVariable->setDefaultValue(fl::nan);
	outputVariable->setLockValidOutput(false);
	outputVariable->setLockOutputRange(false);
	//Set the Fuzzy Membership Functions for the output 
	outputVariable->addTerm(new fl::Gaussian("VeryLeft", -1.000, 0.212));
	outputVariable->addTerm(new fl::Gaussian("Left", -0.500, 0.212));
	outputVariable->addTerm(new fl::Gaussian("Zero", 0.000, 0.212));
	outputVariable->addTerm(new fl::Gaussian("Right", 0.500, 0.212));
	outputVariable->addTerm(new fl::Gaussian("VeryRight", 0.995, 0.212));
	engine->addOutputVariable(outputVariable);
}
void Fuzzy::RuleBlockInit(){
	fl::RuleBlock* ruleBlock = new fl::RuleBlock;
	ruleBlock->setEnabled(true);
	ruleBlock->setName("");
	ruleBlock->setConjunction(new fl::Minimum);
	ruleBlock->setDisjunction(new fl::Maximum);
	ruleBlock->setActivation(new fl::Minimum);
	//Set the rules for the Fuzzy Inference System
	ruleBlock->addRule(fl::Rule::parse("if LineOn is VeryRight and Steering is VeryRight then Direction is Zero", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is VeryRight and Steering is Right then Direction is Right", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is VeryRight and Steering is Zero then Direction is Right", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is VeryRight and Steering is Left then Direction is VeryRight", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is VeryRight and Steering is VeryLeft then Direction is VeryRight", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Right and Steering is VeryLeft then Direction is VeryRight", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Right and Steering is Left then Direction is VeryRight", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Right and Steering is Zero then Direction is Right", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Right and Steering is Right then Direction is Zero", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Right and Steering is VeryRight then Direction is Left", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Zero and Steering is VeryRight then Direction is VeryLeft", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Zero and Steering is Right then Direction is Left", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Zero and Steering is Zero then Direction is Zero", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Zero and Steering is Left then Direction is Right", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Zero and Steering is VeryLeft then Direction is VeryRight", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Left and Steering is VeryLeft then Direction is Right", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Left and Steering is Left then Direction is Zero", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Left and Steering is Zero then Direction is Left", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Left and Steering is Right then Direction is VeryLeft", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is Left and Steering is VeryRight then Direction is VeryLeft", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is VeryLeft and Steering is VeryRight then Direction is VeryLeft", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is VeryLeft and Steering is Right then Direction is VeryLeft", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is VeryLeft and Steering is Zero then Direction is Left", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is VeryLeft and Steering is Left then Direction is Left", engine));
	ruleBlock->addRule(fl::Rule::parse("if LineOn is VeryLeft and Steering is VeryLeft then Direction is Zero", engine));
	engine->addRuleBlock(ruleBlock);
}
Fuzzy::~Fuzzy()
{
}
float Fuzzy::CalculateVelocityDelta(float position, float velocity){
	//set the input valies to be the psoition from the line and the velocity of the car
	inputVariable1->setInputValue(position);
	inputVariable2->setInputValue(velocity);
	//process the data
	engine->process();
	//return the defuzification value
	return outputVariable->defuzzify();
}
void Fuzzy::Init(){
	EngineInit();
	InputVariablesInit();
	OutputVariableInit();
	RuleBlockInit();
}
void Fuzzy::CleanUp(){
	delete inputVariable1;
	inputVariable1 = NULL;

	delete inputVariable2;
	inputVariable2 = NULL;

	delete outputVariable;
	outputVariable = NULL;

	delete engine;
	engine = NULL;
}
void Fuzzy::EngineInit(){
	//Initalise th engine
	engine = new fl::Engine;
	engine->setName("AI");
	engine->addHedge(new fl::Any);
	engine->addHedge(new fl::Extremely);
	engine->addHedge(new fl::Not);
	engine->addHedge(new fl::Seldom);
	engine->addHedge(new fl::Somewhat);
	engine->addHedge(new fl::Very);
}