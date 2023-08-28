#include "Teros10.hpp"

/*
 * Init the pin chosen as an input pin and save it in the class
 */
Teros10::Teros10(int analog_pin, int pwr_pin){

	this->analog_pin = analog_pin;
	this->pwr_pin = pwr_pin;

	pinMode(this->analog_pin, INPUT);
	pinMode(this->pwr_pin, OUTPUT);
	digitalWrite(this->pwr_pin, LOW);

}

// read just the voltage measured by the sensor
float Teros10::readVoltage(){

	digitalWrite(this->pwr_pin, HIGH);
	delay(10);

	int output = analogRead(this->analog_pin); // read a value 0 - 1023 and convert to voltage
	return output;

	digitalWrite(this->pwr_pin, LOW);

}

// read the sensor and return the VWC as a decimal which represents the VWC on the range 0 - 0.77 m^3/m^3
double Teros10::getVWC(SoilMedia media){
	
	float mv = readVoltage();
	double output;


	if(media == MINERAL_SOIL){
		double third_deg = 	0.0000000004824 * pow(mv, 3); 
		double second_deg = 0.000002278 * pow(mv, 2);
		double first_deg = 	0.003898 * mv;
		const double constant = 2.154;

		output = third_deg - second_deg + first_deg - constant;

	}else if(media == SOILLESS){
		double third_deg = 0.0000000005439*pow(mv, 3); 
		double second_deg = 0.000002731*pow(mv, 2);
		double first_deg = (0.004868*mv);
		const double constant = 2.683;

		output = third_deg - second_deg + first_deg - constant;

	}

	return output;
}

// read the sensor and return the VWC as a decimal which represents the VWC on the range 0 - 0.77 m^3/m^3
double Teros10::getVWC(SoilMedia media, double voltage){
	
	double mv = voltage*1000;
	double output;

	if(media == MINERAL_SOIL){
		double third_deg = 	0.0000000004824 * pow(mv, 3); 
		double second_deg = 0.000002278 * pow(mv, 2);
		double first_deg = 	0.003898 * mv;
		const double constant = 2.154;

		output = third_deg - second_deg + first_deg - constant;

	}else if(media == SOILLESS){
		double third_deg = 0.0000000005439*pow(mv, 3); 
		double second_deg = 0.000002731*pow(mv, 2);
		double first_deg = (0.004868*mv);
		const double constant = 2.683;

		output = third_deg - second_deg + first_deg - constant;

	}

	return output;
}

// read dialectric permitivity which can then be converted to VWC using Topp equation
double Teros10::apparentDialectricPermitivity(){

	const double constant = 0.01054;
	const double e = 2.72827;
	const double exp_1 = 0.002827;	

	float mv = readVoltage();
	return constant * pow(e, exp_1 * mv);

}

std::string Teros10::getSensorType(){
	return "meter_teros10";
}
