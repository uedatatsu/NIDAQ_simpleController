/****************************************************************************
*
* 手動で入力電圧を変更できるプログラム
* 各キャリブレーションなどに利用
* z : -delta
* x : +delta
* a : -10*delta
* s : +10*delta
* q : 終了
*
* 作成者：上田龍幸
* 更新日：2019/02/25
*
****************************************************************************/




#include "pch.h"
#include <iostream>
#include <windows.h>

#pragma comment(lib, "NIDAQmx.lib")
#pragma warning(disable:4996)

#define OUTPUT_SAMPS 600  // 1周期分の波形を生成するのに用いるクロック回数
#define OUTPUT_CLOCK_RATE 36000   // 出力1秒間のクロック回数

#define MAX_VOLT 0.070	// 波形出力時の最大電圧[V]
#define MIN_VOLT -0.070	// 波形出力時の最小電圧[V]

#define delta 0.001		// 変化幅[V]


#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

int main()
{
	int32       error = 0;
	TaskHandle  taskHandleAO = 0;
	int32       read = 0;
	static int  totalRead = 0;
	float64     dataAO[OUTPUT_SAMPS] = { 0 };	//入力変数
	float64     datazero[OUTPUT_SAMPS] = { 0 };	//最終初期化用
	char        errBuff[2048] = { '\0' };
	float64		voltage = 0.000;				//初期入力電圧

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk(DAQmxCreateTask("Analog Output", &taskHandleAO));
	DAQmxErrChk(DAQmxCreateAOVoltageChan(taskHandleAO, "Dev2/ao0", "Output", -10.0, 10.0, DAQmx_Val_Volts, ""));	//デバイス名/出力ポートを設定
	DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandleAO, "", (float64)OUTPUT_CLOCK_RATE, DAQmx_Val_Rising, DAQmx_Val_ContSamps, (float64)OUTPUT_SAMPS));
	DAQmxErrChk(DAQmxRegisterDoneEvent(taskHandleAO, 0, DoneCallback, NULL));

	/*********************************************/
	// DAQmx Write Code
	/*********************************************/
	DAQmxErrChk(DAQmxWriteAnalogF64(taskHandleAO, OUTPUT_SAMPS, 0, 10.0, DAQmx_Val_GroupByChannel, dataAO, NULL, NULL));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk(DAQmxStartTask(taskHandleAO));

	for (int i = 0; i < OUTPUT_SAMPS; i++) {
		dataAO[i] = voltage;	//初期化
	}

	DAQmxErrChk(DAQmxWriteAnalogF64(taskHandleAO, OUTPUT_SAMPS, 0, 10.0, DAQmx_Val_GroupByChannel, dataAO, NULL, NULL));	//書き込み
	printf("voltage::%f\r", voltage);
	Sleep(100);		//待ち時間必要

	for (bool flag = true; flag == true;) {
		if (GetAsyncKeyState('Q')) {
			flag = false;
		}

		if (GetAsyncKeyState('Z') && voltage > (float64)(MIN_VOLT)) {
			voltage = voltage - (float64)delta;
			printf("voltage::%f\r", voltage);


			for (int i = 0; i < OUTPUT_SAMPS; i++) {
				dataAO[i] = voltage;
			}

			DAQmxErrChk(DAQmxWriteAnalogF64(taskHandleAO, OUTPUT_SAMPS, 0, 10.0, DAQmx_Val_GroupByChannel, dataAO, NULL, NULL));
			Sleep(100);
		}
		if (GetAsyncKeyState('X') && voltage < (float64)(MAX_VOLT)) {
			voltage = voltage + (float64)delta;
			printf("voltage::%f\r", voltage);

			for (int i = 0; i < OUTPUT_SAMPS; i++) {
				dataAO[i] = voltage;
			}

			DAQmxErrChk(DAQmxWriteAnalogF64(taskHandleAO, OUTPUT_SAMPS, 0, 10.0, DAQmx_Val_GroupByChannel, dataAO, NULL, NULL));
			Sleep(100);
		}

		if (GetAsyncKeyState('A') && voltage > (float64)(MIN_VOLT)) {
			voltage = voltage - (float64)delta*10.0;
			printf("voltage::%f\r", voltage);

			for (int i = 0; i < OUTPUT_SAMPS; i++) {
				dataAO[i] = voltage;
			}

			DAQmxErrChk(DAQmxWriteAnalogF64(taskHandleAO, OUTPUT_SAMPS, 0, 10.0, DAQmx_Val_GroupByChannel, dataAO, NULL, NULL));
			Sleep(100);


		}
		if (GetAsyncKeyState('S') && voltage < (float64)(MAX_VOLT)) {
			voltage = voltage + (float64)delta*10.0;
			printf("voltage::%f\r", voltage);

			for (int i = 0; i < OUTPUT_SAMPS; i++) {
				dataAO[i] = voltage;
			}

			DAQmxErrChk(DAQmxWriteAnalogF64(taskHandleAO, OUTPUT_SAMPS, 0, 10.0, DAQmx_Val_GroupByChannel, dataAO, NULL, NULL));
			Sleep(100);
		}
	}

	DAQmxErrChk(DAQmxWriteAnalogF64(taskHandleAO, OUTPUT_SAMPS, 0, 10.0, DAQmx_Val_GroupByChannel, datazero, NULL, NULL));
	std::cout << "End of program, press Enter key to quit\n" << std::endl;
	Sleep(1000);

	

Error:
	if (DAQmxFailed(error))
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
	if (taskHandleAO != 0) {
		/*********************************************/
		//DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleAO);
		DAQmxClearTask(taskHandleAO);
	}
	if (DAQmxFailed(error))
		printf("DAQmx Error: %s\n", errBuff);
	printf("End of program, press Enter key to quit\n");
	return 0;

}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
	int32   error = 0;
	char    errBuff[2048] = { '\0' };

	// Check to see if an error stopped the task.
	DAQmxErrChk(status);

Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
		DAQmxClearTask(taskHandle);
		std::cout << "DAQmx Error: " << errBuff << std::endl;
	}
	return 0;
}
