#include "Algorithms.h"

int16_t pAtm = 0;                              // ����������� ��������
int16_t p4 = 0;                                // �������� � ������� 4 

int16_t wheelsPressure[6] = { 0 };             // ������� �������� � �������
int16_t wheelsPressure2[6] = { 0 };            // ���������� �������� � �������

int16_t requiredPressure;
uint8_t damagedWheels = 0;                     // ������������ ������, �� ���� ���������
uint8_t disabledWheels = 0;                    // ������, ����������� �� ����������

#define WHEELS_ALL 0xFF

											   ////////////////////  ��������� ������� ////////////////////////////////////////

											   /**
											   \brief ���� ��������.
											   ��������� ����������� �� ������� ����� �����, ������ �� "1".

											   \param i ���������� �������� ��������
											   \param mask ������� �����
											   */
uint8_t GetNextWheel(uint8_t i, uint8_t mask)
{
	mask &= !damagedWheels & GetCircuitStatus();

	int size = WHEELS_COUNT;
	while (i < size && !(mask & (1 << i)))
		i++;

	return i;
}

/*
��������� ���� � ����������.
�������������:
WHEELS_ITERATOR(i, my_mask)
...
*/
#define WHEELS_ITERATOR(I, MASK)for(uint8_t (I) = GetNextWheel(0, (MASK)); (I)<(WHEELS_COUNT); (I) = GetNextWheel((I)+1, (MASK)))




void A1_CheckValve(uint16_t valve, bool* kx_error, bool* k10_error);

// ��������� �������� � 4� �������
int16_t getCircuit4Pressure(void);

// ��������, ��� �������� ����������
bool CheckCircuit4Pressure(void);

//��������� ��������
//4 ������ � ����������� 0.1 �, � ����� ����������
int16_t measurePressure(void);

//��������� ���������� ������� ��������
uint16_t calcUpQuants(uint16_t currentPressure, uint16_t requiredPressure);

// ��������� ���������� ������� ������
uint16_t calcDownQuants(uint16_t currentPressure, uint16_t requiredPressure);

// ������ ������� ���������, ���������� ��������, ����� � ������ �������������
// ����� ��������
void WaitForPressureDone(uint16_t* quantsNumber, uint16_t maxQuant, int quant);

void B1_ForTime(uint8_t wheels, int seconds);

////////////////////////////////////////////////////////////////////////////////

//���������������  ������� ��� ���������
AlgResult A0(void)
{
	AlgResult result;

	result = A1();
	if (result != ALG_OK)return result;

	result = A2();
	if (result != ALG_OK)return result;

	result = A3();
	if (result != ALG_OK)return result;

	result = A4();
	if (result != ALG_OK)return result;

	result = A5();
	if (result != ALG_OK)return result;

	A6(Wheels_All);

	return ALG_OK;
}

//�������� ����������������� �������� ���������� ������� ��������� �8 � �9
AlgResult A1(void)
{
	// ���������� �������
	bool k8_error = false, k9_error = false, k10_error = false;
	bool critical_error = false;

	SetValves(Valves_None);

	// �������� �������� � ������� 4
	if (!CheckCircuit4Pressure())
		return ALG_CRITICAL_ERROR;


	/* �������� ������� �8:
	����� �������� �������� ������ �������������,
	� ����� �������� - ��������� � ���������*/
	A1_CheckValve(K8_Valve, &k8_error, &k10_error);

	if (!k10_error)
	{
		/* �������� ������� �9.
		����� �������� �������� ������ �����������,
		� ����� ��������� */
		A1_CheckValve(K9_Valve, &k9_error, &k10_error);
	}

	// ���� �������� ��� ������� �8 � �9 - ����������� ������  
	critical_error = k10_error | (k8_error & k9_error);
	//CanSendK8K9K10Status(critical_error, k8_error, k9_error, k10_error);

	if (critical_error) return ALG_CRITICAL_ERROR;
	if (k8_error | k9_error) return ALG_ERROR;
	return ALG_OK;
}


// ��������� ���� �8, ���� �9
void A1_CheckValve(uint16_t valve, bool* kx_error, bool* k10_error)
{
	int16_t p[2];

	p[0] = VDAT_0;                      // ����������� �������� �� ��������
	SetValve(valve, VALVE_OPEN);
	uint8_t isIncreasing;

	vTaskDelay(1000);

	//����������� �� ��������?
	p[1] = GetAdcValue();
	isIncreasing = p[1] - p[0] > EPS_RISING;

	// ���� �������� �������������
	if (isIncreasing)
	{
		SetValve(valve, VALVE_CLOSE);
		SetValve(K10_Valve, VALVE_OPEN);
		vTaskDelay(2000);

		if (abs(p[0] - GetAdcValue()) > EPS_EQUAL)
		{
			// ���� �������� �� ��������� � ��������������� ��������� - ����� ������� �10
			// ����������� ������
			*k10_error = true;
		}

		SetValve(K10_Valve, VALVE_CLOSE);
	}
	else
	{
		// ����� ������� �8
		*kx_error = true;
		SetValve(valve, VALVE_CLOSE);
	}
}



//��������� ������������ �������� �0 � ������������� � ������� ��������
AlgResult A2(void)
{
	SetValves(Valves_None);
	SetValve(K10_Valve, VALVE_OPEN);

	vTaskDelay(3000);			//TODO: ��� ������� - 3000, ������� - 30000

	int16_t pressure = measurePressure();
	bool isPressureOk = (pressure <= V1_1) && (pressure >= V0_4);	 // 0.4 - 1.1�

	//CanSendPressureSensorStatus(isPressureOk, pressure);

	if (isPressureOk)
		pAtm = pressure;

	SetValve(K10_Valve, VALVE_CLOSE);

	return isPressureOk ? ALG_OK : ALG_CRITICAL_ERROR;
}



//��������� ���������� �������� � �������� ����������
AlgResult A3(void)
{
	bool isPressureOk;
	AlgResult result = ALG_OK;

	SetValves(Valves_None);
	SetValve(K8_Valve, VALVE_OPEN);
	SetValve(K9_Valve, VALVE_OPEN);

	int16_t p1 = 0, p2 = 0;
	TickType_t t0 = xTaskGetTickCount();

	// ���� �������� �� ���������� �� 6 ��/��2
	do
	{
		if (xTaskGetTickCount() - t0 >= 8000) //TODO: 5 * 60 * 1000
		{
			//���� �� 5 ����� �� �����, �� ������������� ������
			//TODO: �� ����. ������. ��������� ������
			result = ALG_ERROR;
			break;
		}

		vTaskDelay(2500);
		p1 = GetAdcValue();
	} while (p1 < P_6KG_SM2);


	// ���� �������� �� ���������������
	if (result == ALG_OK)
	{
		do
		{
			vTaskDelay(5000);
			p2 = GetAdcValue();
			isPressureOk = (p2 - p1 < DP_0_1KG_SM2);       // < 0.1 ��/��2  

			if (!isPressureOk)
			{
				p1 = p2;
			}

		} while (!isPressureOk);
	}

	// ���������� ��������
	SetValve(K8_Valve, VALVE_CLOSE);
	SetValve(K9_Valve, VALVE_CLOSE);
	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(2000);
	SetValve(K10_Valve, VALVE_CLOSE);

	//CanSendInitialPressureStatus(p2);

	return result;
}



//�������� ������������� ��� �� ���������� ��������
AlgResult A4(void)
{
	AlgResult result = ALG_OK;

	SetValves(Valves_None);

	SetValve(K8_Valve, VALVE_OPEN);
	SetValve(K9_Valve, VALVE_OPEN);
	vTaskDelay(500);
	SetValve(K8_Valve, VALVE_CLOSE);
	SetValve(K9_Valve, VALVE_CLOSE);
	vTaskDelay(2000);

	// �������� ���������
	int16_t pressure1 = measurePressure();

	if (abs(pressure1 - p4) >= DP_0_2KG_SM2)      // < 0.2 ��/��2 
	{
		result = ALG_ERROR;
		// �� ����������� ������. �� � ��� �� ����?
		//TODO: �������� �������
	}


	vTaskDelay(4000); //TODO: 60 * 1000
	int16_t pressure2 = measurePressure();

	if (abs(pressure1 - pressure2) >= DP_0_1KG_SM2)
	{
		//�� ������������� ��� �� ���������� ��������
		result = ALG_CRITICAL_ERROR;
		//CanSendTigthnessPressureStatus(false);
	}
	else
		//CanSendTigthnessPressureStatus(true);


	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(2000);
	SetValve(K10_Valve, VALVE_CLOSE);

	return result;
}



//�������� ������������� ��� �� ������
AlgResult A5(void)
{
	AlgResult result = ALG_OK;

	SetValves(Valves_None);

	SetValve(K7_Valve, VALVE_OPEN);
	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(5000);
	SetValve(K10_Valve, VALVE_CLOSE);
	vTaskDelay(1000);
	SetValve(K7_Valve, VALVE_CLOSE);
	vTaskDelay(2000);

	// �������� ���������
	int16_t pressure1 = measurePressure();

	if (pressure1 < pAtm)
	{
		vTaskDelay(6 * 1000);		//TODO: ��� ������� - 6000, ������� - 60000
		int16_t pressure2 = measurePressure();

		if (pressure2 - pressure1 >= DP_0_05KG_SM2)
		{
			//�� ������������� ��� �� ������
			result = ALG_CRITICAL_ERROR;
			//CanSendTigthnessVakuumStatus(false);
		}
		else
		{
			//CanSendTigthnessVakuumStatus(true);
		}

		SetValve(K10_Valve, VALVE_OPEN);
		vTaskDelay(2000);
		SetValve(K10_Valve, VALVE_CLOSE);
	}
	else
	{
		result = ALG_CRITICAL_ERROR;
		//CanSendTigthnessVakuumStatus(false);
	}

	return result;
}



//��������� �������� �������� ������� � �����
void A6(uint8_t wheels)
{
	SetValves(Valves_None);

	for (int i = 0; i < 6; i++)
		WHEELS_ITERATOR(i, wheels)
	{
		// P���i_2 = P���i_���
		wheelsPressure2[i] = wheelsPressure[i];

		vTaskDelay(500);

		//SetValves((1 << i) | K8_Valve | K9_Valve);
		SetValve(1 << i, VALVE_OPEN);
		SetValve(K8_Valve, VALVE_OPEN);
		SetValve(K9_Valve, VALVE_OPEN);

		vTaskDelay(900);

		SetValve(K8_Valve, VALVE_CLOSE);
		SetValve(K9_Valve, VALVE_CLOSE);

		vTaskDelay(2000);
		wheelsPressure[i] = measurePressure();
		SetValve(1 << i, VALVE_CLOSE);
	}

	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(2000);
	SetValve(K10_Valve, VALVE_CLOSE);

	//CanSendPressure(wheelsPressure);
}



//������ ������� � �������� ������
//���������� ���������� �� ������� ������
//values ������� �� ������������ ������ (???), ���� �������� �������
void B0(int16_t requiredWheelsPressure)
{
	// ������� �������� ������
	int pumpUpCnt = 0;

	//requiredWheelsPressure = reqPressure;

	A6(Wheels_All);

	// ���������� ����������� ��������� "����������� ����������� ������" �0
	//G0();

	while (true)
	{
		uint8_t Kn = 0; //�����, ����� ������ ���������
		uint8_t Ks = 0; //�����, ����� ������ ��������

						// �����������, ����� ������ ���������, � ����� ��������
		WHEELS_ITERATOR(i, WHEELS_ALL)
		{
			if (requiredWheelsPressure - wheelsPressure[i] > DP_0_2KG_SM2)
				Kn |= 1 << i;
			else if (wheelsPressure[i] - requiredWheelsPressure > DP_0_2KG_SM2)
				Ks |= 1 << i;
		}

		if (Kn)
		{
			// ���������
			//B1(Kn, requiredPressure);

			// ���� ��������� ����������� 3� ��� ������ - ��������
			pumpUpCnt++;
			if (pumpUpCnt >= 3)
			{
				vTaskDelay(2000); //TODO: 2*60*1000
				return;
			}
		}
		else if (Ks)
		{
			//��������
			//B2(Ks, requiredPressure);
			pumpUpCnt = 0;
		}
		else
		{
			vTaskDelay(5000); //TODO: 5*60*1000
			return;
		}
	}
}



//��������� �������� ������� � ������ (-��)
void B1(uint8_t wheels, uint16_t requiredPressure)
{
	uint16_t quantsNumber[6] = { 0 };     // ����� �������� ������� ������
	uint16_t maxQuant = 0;              // ������������ ����� ��������
	uint8_t currentUp = 0;

	// ���������� ������� �������� � ��������� ������� �����, ����� ������ �����������
	WHEELS_ITERATOR(i, wheels)
	{
		quantsNumber[i] = calcUpQuants(wheelsPressure[i], requiredPressure);
		if (quantsNumber[i] >= maxQuant)
			maxQuant = quantsNumber[i];

		if (quantsNumber[i])
			currentUp |= (1 << i);
	}

	// ��������� �������
	SetValves(currentUp | K8_Valve | K9_Valve);

	// �����������
	WaitForPressureDone(quantsNumber, maxQuant, P_UP_QUANT);

	SetValves(Valves_None);

	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(2000);
	SetValve(K10_Valve, VALVE_CLOSE);

	if (A3() == ALG_OK)
		A6(Wheels_All);
}

// ����������� � ������� ��������� ����� ������
void B1_ForTime(uint8_t wheels, int seconds)
{
	// ��������� �������
	WHEELS_ITERATOR(i, wheels)
		SetValve(ValveFromNumber(i), VALVE_OPEN);

	SetValve(K8_Valve, VALVE_OPEN);
	SetValve(K9_Valve, VALVE_OPEN);

	// �����������
	vTaskDelay(seconds * 1000);

	// ���������
	SetValve(K8_Valve, VALVE_CLOSE);
	SetValve(K9_Valve, VALVE_CLOSE);

	WHEELS_ITERATOR(i, wheels)
		SetValve(ValveFromNumber(i), VALVE_CLOSE);
}

//�������� �������� ������� � ������ (-��)
void B2(uint8_t wheels, uint16_t requiredPressure)
{
	uint16_t quantsNumber[6] = { 0 };     // ����� ������ ������� ������
	uint16_t maxQuant = 0;              // ������������ ����� ������
	uint8_t currentDown = 0;

	// ���������� ������� ������� � ��������� ������� �����, ����� ������ �������
	WHEELS_ITERATOR(i, wheels)
	{
		quantsNumber[i] = calcDownQuants(wheelsPressure[i], requiredPressure);
		if (quantsNumber[i] >= maxQuant)
			maxQuant = quantsNumber[i];

		if (quantsNumber[i])
			currentDown |= (1 << i);
	}

	// ��������� �������
	SetValves(currentDown | K7_Valve | K10_Valve);

	// ��������
	WaitForPressureDone(quantsNumber, maxQuant, P_DOWN_QUANT);

	SetValves(Valves_None);
	SetValves(K8_Valve | K9_Valve);
	vTaskDelay(700);
	SetValves(Valves_None);

	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(2000);
	SetValve(K10_Valve, VALVE_CLOSE);

	if (A3() == ALG_OK)
		A6(Wheels_All);
}


// ������ ������� ���������, ���������� ��������, ����� � ������ �������������
// ����� ��������
void WaitForPressureDone(uint16_t* quantsNumber, uint16_t maxQuant, int quant)
{
	for (int i = 0; i < maxQuant; i++)
	{
		vTaskDelay(quant);

		// ���������, ����� ������ ���������� � ��������� ��
		for (int j = 0; j < 6; j++)
		{
			if (quantsNumber[j] != 0 && !(quantsNumber[j] > i + 1))
			{
				SetValve(1 << j, VALVE_CLOSE);
				quantsNumber[j] = 0;
			}
		}
	}
}


//����������� ����������� ������
void G0(void)
{
	uint16_t wheelsPressure1[6] = { 0 };

	// ���������� �������, ������� �������� �������� - ����� ����� ���������� 
	// ����������� �� �������� ����� ���� ������
	uint8_t wheelsToCheck = 0;
	WHEELS_ITERATOR(i, WHEELS_ALL)
		wheelsToCheck |= (wheelsPressure2[i] - wheelsPressure[i] < DP_0_3KG_SM2) << i;   // < 0.3 ��/��2

	if (!wheelsToCheck)
		return;

	// ����� ���������
	A6(wheelsToCheck);

	WHEELS_ITERATOR(i, wheelsToCheck)
	{
		wheelsPressure1[i] = wheelsPressure[i];
	}

	vTaskDelay(10000);

	A6(wheelsToCheck);

	/* �� ���������� ���������:
	wheelsPressure  = Pti
	wheelsPressure2 = Pti-1
	wheelsPressure1 = Pti-2
	*/

	// ����������� ������ ������������ �����
	damagedWheels = 0;
	WHEELS_ITERATOR(i, wheelsToCheck)
	{
		bool isDamaged = (wheelsPressure2[i] - wheelsPressure[i] > DP_0_1KG_SM2) || 		// > 0.1 ��/��2
			(wheelsPressure1[i] - wheelsPressure[i] > DP_0_15KG_SM2);       // > 0.15 ��/��2

		damagedWheels |= isDamaged << i;
	}

	if (damagedWheels)
	{
		//TODO: ��������� ������ � ����������� �����s
		D0(damagedWheels);
	}
}

void D0(uint8_t damagedWheels)
{

	uint16_t startPressures[6];
	WHEELS_ITERATOR(i, damagedWheels)
		startPressures[i] = wheelsPressure[i];

	B1_ForTime(damagedWheels, 2 * 60);
	A6(damagedWheels);
	B1_ForTime(damagedWheels, 2 * 60);
	A6(damagedWheels);

	do
	{

		// ���������� �� �������������� �������
		WHEELS_ITERATOR(i, damagedWheels)
		{
			if (startPressures[i] - wheelsPressure[i] > DP_0_5KG_SM2) // > 0.5 ��/��2
			{
				//��������� ������ �� ����������
				disabledWheels |= (1 << i);

				// ��������� �� ������ ������������
				damagedWheels &= ~(1 << i);
			}
		}

		if (damagedWheels)
		{
			B1_ForTime(damagedWheels, 6 * 60);
			A6(damagedWheels);

			WHEELS_ITERATOR(i, damagedWheels)
			{
				if ((wheelsPressure[i] - requiredPressure >= DP_1KG_SM2)) // >= 1 ��/��2
				{
					// ��������� �� ������ ������������
					damagedWheels &= ~(1 << i);
				}
			}
		}

	} while (damagedWheels);

}

// �������� �������� ����� � ������� ��������� �������
void PumpUpDamagedWheels(uint8_t damagedWheels, TickType_t time)
{
	// ��������� �������
	SetValves(damagedWheels | K8_Valve | K9_Valve);

	// �����������
	vTaskDelay(time);

	SetValves(Valves_None);

	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(2000);
	SetValve(K10_Valve, VALVE_CLOSE);
}

///////////////////// ��������� ������� ////////////////////////////////////////

//�������� �������� � ��������� �������
//�������, ���� �������� � 4 ������� �� ����� ���� ���������� ���������� ��������
int16_t getCircuit4Pressure(void)
{
	uint8_t buffer[COMMAND_LENGTH] = { 0 };
	int16_t circuit4Pressure;

	buffer[0] = CAN_CMD_Circuit_Request;
	CAN_Send_Command(buffer);
	CAN_GetContour4Response(&circuit4Pressure, portMAX_DELAY);

	p4 = circuit4Pressure;
	return circuit4Pressure;
}


// ��������, ��� �������� � ������� 4 ����������
bool CheckCircuit4Pressure(void)
{
	//�������� �������� � ������� 4
	//���� �������� � ������� 5� ����� �� ��������� ����������,
	//�� ����������� ������

	TickType_t t0 = xTaskGetTickCount();
	while (getCircuit4Pressure() <= NORMAL_PRESSURE)
	{
		// ������ ����� 5� �����
		if (xTaskGetTickCount() - t0 > 5 * 60 * 1000)
		{
			return false;
		}

		vTaskDelay(1000);
	}

	return true;
}

//��������� ��������
//4 ������ � ����������� 0.1 �, � ����� ����������
int16_t measurePressure(void)
{
	uint16_t pressure = 0;
	for (int i = 0; i < 4; i++)
	{
		pressure += GetAdcValue();
		vTaskDelay(100);
	}
	pressure = pressure >> 2;
	return pressure;
}

//��������� ���������� ������� ��������
uint16_t calcUpQuants(uint16_t currentPressure, uint16_t requiredPressure)
{
	double numQuants = 1.0;
	double a = P_COMPRESSOR - requiredPressure;
	double b = P_COMPRESSOR - currentPressure;
	double c = log(a / b);
	double d = -52.0 * c;
	//numQuants = -206.0 * log((P_COMPRESSOR - requiredPressure) / (P_COMPRESSOR - wheelsPressure[wheel]));
	double e = (uint16_t)(floor(d) + 1);
	return e;
}

// ��������� ���������� ������� ������
uint16_t calcDownQuants(uint16_t currentPressure, uint16_t requiredPressure)
{
	return 0;
}
