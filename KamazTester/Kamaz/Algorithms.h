/*
���������� ����������
*/

#ifndef __ALGORITHMS_H__
#define __ALGORITHMS_H__

#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include <API\api.h>

#define WHEELS_COUNT            6

/* ������� ��� �������� ��������
���� � �������         N = V / 5.1 * 4096
���� � ��� (��/��2)    N = (p + 1) * 321 + 482
���� ������� ��������  N = (dp / 1) * 321
*/

#define EPS_EQUAL               200                     // ����������� ����������� �������� � ����� (�1)
#define EPS_RISING              30                      // ����������� ���������� �������� (�1)
#define NORMAL_PRESSURE         4                       // ���������� ��������, 4 ��/��2 (A1)
#define V1_1                    883                     // ������� � ������ ������� ������������ �������� (�2)
#define V0_4                    321                     // 0.4 - 1.1�        	   
#define P_6KG_SM2               2729                    // 6 ��/��2  (�3)
#define DP_0_1KG_SM2            32                      // ������� � 0.1 ��/��2(A3, A4, �0)
#define DP_0_2KG_SM2            64                      // ������� � 0.2 ��/��2(A4, �0)
#define DP_0_05KG_SM2           16                      // ������� � 0.05 ��/��2 (A5)
#define T_PRES_UP_MIN           2000                    // ����������� � ������������ ����� �������� (�1)
#define T_PRES_UP_MAX           120000                  // �� 2 � �� 2 ���
#define T_PRES_DOWN_MIN         2000                    // ����������� � ������������ ����� ������ (�2)
#define T_PRES_DOWN_MAX         45000                   // �� 14 � �� 45 �
#define DP_0_3KG_SM2            96                      // ������� � 0.3 ��/��2  (�0)
#define DP_0_15KG_SM2           48                      // ������� � 0.15 ��/��2 (�0)
#define DP_0_5KG_SM2            160                     // ������� � 0.5 ��/��2 (�0)
#define DP_1KG_SM2              321                     // ������� � 1 ��/��2 (�0)

#define P_UP_QUANT              2000                    // ����� ������� ��������
#define P_DOWN_QUANT            10                      //??? // ����� ������� ������
#define P_COMPRESSOR            2729                    //??? �������� � ����������� (6 ���)
#define T_PAUSE                 500                     // ����� �����

#define VDAT_0                  1                       //����������� �������� �� �������� TODO: ����������

typedef enum
{
	ALG_OK,
	ALG_ERROR,
	ALG_CRITICAL_ERROR
} AlgResult;



//���������������  ������� ��� ���������
AlgResult A0(void);

//�������� ����������������� �������� ���������� ������� ��������� �8 � �9
AlgResult A1(void);

//��������� ������������ �������� �0 � ������������� � ������� ��������
AlgResult A2(void);

//��������� ���������� �������� � �������� ����������
AlgResult A3(void);

//�������� ������������� ��� �� ���������� ��������
AlgResult A4(void);

//�������� ������������� ��� �� ������
AlgResult A5(void);

//��������� �������� �������� ������� � �����
void A6(uint8_t wheels);

//������ ������� � �������� ������
void B0(int16_t requiredWheelsPressure);

//��������� �������� ������� � ������ (-��)
void B1(uint8_t wheels, uint16_t requiredPressure);

//�������� �������� ������� � ������ (-��)
void B2(uint8_t wheels, uint16_t requiredPressure);

//����������� ����������� ������
void G0(void);

void D0(uint8_t damagedWheels);

#endif
