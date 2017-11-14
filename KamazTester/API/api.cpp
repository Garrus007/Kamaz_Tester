#include "api.h"

// ������� ���������
InputStore<uint16_t> circuitState;			// ����� �������
InputStore<float> budPressure;				// �������� � ������� (== �������� �������)
InputStore<float> contour4Pressure;			// �������� � �������

// ���������
int currentTime;						// ������� ����� � �����
uint16_t valvesState;					// ����� ��������

// ����������� ��������
vector<string> actions;


int16_t atmToAdc(float atm);
float adcToAtm(int16_t adc);
void setValves(vector<bool> valves);
string valvesToString();

/////////////////////////// RTOS /////////////////////////////////////////
void vTaskDelay(int delay)
{	
	currentTime += delay;
	actions.push_back(DELAY + std::to_string(delay));
}

TickType_t xTaskGetTickCount(void)
{
	return currentTime;
}


/////////////////////////// VALVES ///////////////////////////////////////
//���������� ������ �� ������
uint16_t ValveFromNumber(uint8_t valveNumber)
{
	//� ������� ������� �������� - ������� �����, ��� 1 �� ������ �������
	//���� ��������� � �������, �� ��� ���������
	return 1 << (valveNumber - 1);
}

//���������� ����� �� �������
uint8_t NumberFromValve(uint16_t valve)
{
	// ������ �� 0 �� 9
	uint8_t number;
	switch (valve)
	{
	case K1_Valve:
		number = 0;
		break;
	case K2_Valve:
		number = 1;
		break;
	case K3_Valve:
		number = 2;
		break;
	case K4_Valve:
		number = 3;
		break;
	case K5_Valve:
		number = 4;
		break;
	case K6_Valve:
		number = 5;
		break;
	case K7_Valve:
		number = 6;
		break;
	case K8_Valve:
		number = 7;
		break;
	case K9_Valve:
		number = 8;
		break;
	case K10_Valve:
		number = 9;
		break;
	}
	return number;
}

//��������� � ��������� ������� �� ������ ������� �����
void SetValves(uint16_t valves)
{
	valvesState = valves;
	actions.push_back(valvesToString());
}

//���������/��������� ���� ������
void SetValve(uint16_t valve, uint8_t state)
{
	if (state == VALVE_OPEN)
		valvesState |= valve;
	else if(state == VALVE_CLOSE)
		valvesState &= ~valve;

	actions.push_back(valvesToString());
}


/////////////////////////// VALVES POWER /////////////////////////////////
uint16_t GetCircuitStatus(void)
{
	actions.push_back(GET_CIRCUIT_STATUS);
	return circuitState.GetNext();
}


/////////////////////////// ADC //////////////////////////////////////////
int16_t GetAdcValue(void)
{
	actions.push_back(ADC);
	return atmToAdc(budPressure.GetNext());
}


/////////////////////////// CAN //////////////////////////////////////////

long CAN_GetContour4Response(void* pvBuffer, TickType_t xTicksToWait)
{
	int16_t pressure = atmToAdc(contour4Pressure.GetNext());
	memcpy(pvBuffer, &pressure, sizeof(pressure));
	actions.push_back(GET_CONTOUR_4_RESPONSE);

	return 0;
}

void CAN_Send_Command(uint8_t* buffer)
{
	actions.push_back(SEND_CONTOUR_4_REQUEST);
}


void CanJ1939_SendCircuit4Error()
{
	actions.push_back(J1939_CIRCUIT_4_ERROR);
}

void CanJ1939_SendK10Error()
{
	actions.push_back(J1939_K10_ERROR);
}

void CanJ1939_SendK8AndK9Error()
{
	actions.push_back(J1939_K8_K9_ERROR);
}

void CanJ1939_SendK8Error()
{
	actions.push_back(J1939_K8_ERROR);
}

void CanJ1939_SendK9Error()
{
	actions.push_back(J1939_K9_ERROR);
}

void CanJ1939_SendSensorNotCalibratedError()
{
	actions.push_back(J1939_SENSOR_NOT_CALIBRATED_ERROR);
}

void CanJ1939_SendPressureNotEnoughFor5MinutesError()
{
	actions.push_back(J1939_PRESSURE_NOT_ENOUGH_ERROR);
}

void CanJ1939_SendTightnessPressureError()
{
	actions.push_back(J1939_TIGHTNESS_PRESSURE_ERROR);
}

void CanJ1939_SendTightnessVakuumError()
{
	actions.push_back(J1939_TIGHTNESS_VAKUUM_ERROR);
}


//////////////////////////////////////////////////////////////////////////


// ���������� ����� �������������
void ResetEnv()
{
	actions.clear();
	circuitState.Reset();
	budPressure.Reset();
	contour4Pressure.Reset();
}


vector<string> & getActions()
{
	return actions;
}

// ����������� �������� � ���������� � �������� ���
int16_t atmToAdc(float atm)
{
	return (atm + 1) * 321 + 482;
}

// ����������� �������� ��� � �������� � ����������
float adcToAtm(int16_t adc)
{
	return (adc - 482) / 321 - 1;
}

// ���������� ����� �� ������� ��������
uint16_t getBoolMask(vector<bool> valves)
{
	uint16_t mask = 0;
	for (int i = 0; i < 10; i++)
		mask |= (1 << i);

	return mask;
}

//������������� ���������� �������� 
void setValves(vector<bool> valves)
{
	valvesState = getBoolMask(valves);
}

// ������� ��������� ��������
string valvesToString(uint16_t valves)
{
	stringstream s;
	bool isOpen;

	for (int i = 0; i < 9; i++)
	{
		isOpen = (valvesState & (1 << i)) != 0;
		s << "K" << i + 1 << ": " << isOpen << " ";
	}

	isOpen = (valvesState & (1 << 9)) != 0;
	s << "K" << 10 << ": " << isOpen << " ";

	return s.str();
}

// ������� ��������� ��������
string valvesToString()
{
	return valvesToString(valvesState);
}

