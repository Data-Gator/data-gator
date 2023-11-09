/**
 * @file 74HC40520.hpp
 * @brief Implements library for an analog signal multiplexer.
 *
 * @author Garrett Wells
 * @date 2022
 */
#include <Arduino.h>

/**
 * @brief Defines a firmware interface for the 74HC40520 multiplexer.
 *
 * ### Chip Pin Truth Table
 * 
 *	| MUX_INH  | B     | A     | Selected Channels | 
 *	| :------: | :---: | :---: | :---------------: | 
 *	| L        | L     | L     | 0X, 0Y
 *  | L        | L     | H     | 1X, 1Y
 *  | L        | H     | L     | 2X, 2Y
 *  | L        | H     | H     | 3X, 3Y
 *  | H        | X     | X     | XXXXXX
 *
 */
class MUX_74HC40520{
public:
	MUX_74HC40520(void){}

	/**
	 * @brief      Sets the output level of the two selector pins (A&B) and the inhibitor pin and stores the pin values.
	 *
	 * @param[in]  mux_inh  The multiplexer inhibitor pin, if HIGH no channel is connected to X/Y pins.
	 * @param[in]  mux_a    The multiplexer a selector pin
	 * @param[in]  mux_b    The multiplexer b selector pin
	 */
	void setPins(int mux_inh, int mux_a, int mux_b){
		this->mux_inh = mux_inh;
		this->mux_a = mux_a;
		this->mux_b = mux_b;

		pinMode(mux_inh, OUTPUT);
		digitalWrite(mux_inh, LOW);

		pinMode(mux_a, OUTPUT);
		digitalWrite(mux_a, LOW);

		pinMode(mux_b, OUTPUT);
		digitalWrite(mux_b, LOW);
	}

	/**
	 * @brief      Select the channels to connect to the X and Y output pins
	 *
	 * @param[in]  channel  The channel
	 */
	void select(int channel){

		if(channel >= 4 || channel < 0){
			if(Serial && DEBUG){
				Serial.println("ERROR: channel is invalid, use one 0-3");
			}
			return;
		}

		switch(channel){
			case 0: // X0, Y0
				digitalWrite(this->mux_inh, LOW);
				digitalWrite(this->mux_a, LOW);
				digitalWrite(this->mux_b, LOW);
				break;
			case 1: // X1, Y1
				digitalWrite(mux_inh, LOW);
				digitalWrite(mux_a, HIGH);
				digitalWrite(mux_b, LOW);
				break;
			case 2: // X2, Y2
				digitalWrite(mux_inh, LOW);
				digitalWrite(mux_a, LOW);
				digitalWrite(mux_b, HIGH);
				break;
			case 3: // X3, Y3
				digitalWrite(mux_inh, LOW);
				digitalWrite(mux_a, HIGH);
				digitalWrite(mux_b, HIGH);
				break;

			default: // turn X and Y off
				digitalWrite(mux_inh, HIGH);
				if(Serial){
					Serial.println("ERROR: invalid MUX channel chosen");
				}
		}
	}

	/**
	 * @brief      Reads the analog value at the channel.
	 *
	 * @param[in]  outputChannel  The output channel, can be `enum Mux_Channel`
	 */
	int readChannel(int pin){
		return analogRead(pin);
	}

	/**
	 * @brief      Writes a channel.
	 *
	 * @param[in]  inputChannel  The input channel
	 * @param[in]  value         The integer value to write to the pin, HIGH or LOW.
	 */
	void writeChannel(int pin, int value){
		digitalWrite(pin, value);
	}

    /**
     * @brief Defines the two MUX signal channels X and Y.
     *
     * Use these channesl to select which channel you want to write
     * or read a value to/from.
     */
	enum Mux_Channel {
		X, //!< X channel
        Y  //!< Y channel
	};

private:
	uint8_t mux_inh, mux_a, mux_b;
};
