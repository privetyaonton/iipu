#pragma once
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "powrprof.lib")
#include <Windows.h>
#include <ioapiset.h>
#include <iostream>
#include <SetupAPI.h>
#include <powrprof.h>
#include <poclass.h>
#include <devguid.h>
#include <thread>
#include <conio.h>

using namespace std;

void GetTypeBattery();
void thread_func();

char input_char = '3';


int main()
{
	thread thr(thread_func);

	while (input_char != 0)
	{
		input_char = _getch();

		if (input_char)
		{
			input_char -= '0';

			switch (input_char)
			{
			case 0: break;

			case 1: SetSuspendState(FALSE, FALSE, FALSE); break;

			case 2: SetSuspendState(TRUE, FALSE, FALSE); break;
			}
		}
	}

	thr.join();
	
	
	return 0;
}

void GetTypeBattery()
{
	/* HDEVINFO DeviceInfoSet;
	DeviceInfoSet = SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData = { 0 };
	ZeroMemory(&DeviceInterfaceData, sizeof(SP_DEVINFO_DATA));
	DeviceInterfaceData.cbSize = sizeof(SP_DEVINFO_DATA);

	SetupDiEnumDeviceInterfaces(DeviceInfoSet, NULL, &GUID_DEVCLASS_BATTERY, 0, &DeviceInterfaceData);
	DWORD cbRequired = 0;

	SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, NULL, NULL, &cbRequired, NULL);
	PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, cbRequired);
	pdidd->cbSize = sizeof(*pdidd);

	SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, pdidd, cbRequired, &cbRequired, NULL);
	HANDLE hBattery = CreateFile(pdidd->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	BATTERY_QUERY_INFORMATION BatteryQueryInformation = { 0 };
	DWORD bytesWait = 0;
	DWORD bytesReturned = 0;
	DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_TAG, &bytesWait, sizeof(bytesWait), &BatteryQueryInformation.BatteryTag,
		sizeof(BatteryQueryInformation.BatteryTag), &bytesReturned, NULL) && BatteryQueryInformation.BatteryTag;

	BATTERY_INFORMATION BatteryInfo = { 0 };
	BatteryQueryInformation.InformationLevel = BatteryInformation;

	DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION, &BatteryQueryInformation, sizeof(BatteryQueryInformation),
		&BatteryInfo, sizeof(BatteryInfo), &bytesReturned, NULL);

	for (int b = 0; b < 4; b++)
	{
		cout << (int)BatteryInfo.Chemistry[b] << " ";
	}
	cout << endl;

	LocalFree(pdidd);
	SetupDiDestroyDeviceInfoList(DeviceInfoSet);*/
	HDEVINFO DeviceInfoSet = SetupDiGetClassDevsA(&GUID_DEVCLASS_BATTERY, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (DeviceInfoSet != INVALID_HANDLE_VALUE)
	{
		SP_DEVICE_INTERFACE_DATA DeviceInterfaceData = { 0 };
		ZeroMemory(&DeviceInterfaceData, sizeof(SP_DEVINFO_DATA));
		DeviceInterfaceData.cbSize = sizeof(SP_DEVINFO_DATA);

		SetupDiEnumDeviceInterfaces(DeviceInfoSet, NULL, &GUID_DEVCLASS_BATTERY, 0, &DeviceInterfaceData);

		DWORD RequiredSize = 0;
		SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, NULL, NULL, &RequiredSize, NULL);

		PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, RequiredSize);
		DeviceInterfaceDetailData->cbSize = sizeof(*DeviceInterfaceDetailData);

		SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, DeviceInterfaceDetailData, RequiredSize, &RequiredSize, NULL);

		HANDLE hDevice = CreateFile(DeviceInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hDevice != INVALID_HANDLE_VALUE)
		{
			BATTERY_QUERY_INFORMATION BatteryQueryInformation = { 0 };

			DWORD dwWait = 0;
			DWORD dwOut = 0;

			DeviceIoControl(hDevice, IOCTL_BATTERY_QUERY_TAG, &dwWait, sizeof(dwWait), &BatteryQueryInformation.BatteryTag, sizeof(BatteryQueryInformation.BatteryTag), &dwOut, NULL);

			BATTERY_INFORMATION BatteryInfo = { 0 };
			BatteryQueryInformation.InformationLevel = BatteryInformation;

			BOOL WorkFunction = DeviceIoControl(hDevice, IOCTL_BATTERY_QUERY_INFORMATION, &BatteryQueryInformation, sizeof(BatteryQueryInformation), &BatteryInfo, sizeof(BatteryInfo), &dwOut, NULL);

			//if (WorkFunction == ERROR_INVALID_FUNCTION) cout << "Unable to determine battery type" << endl;
			//else
			//{
			if (BatteryInfo.Chemistry[0])
			{
				for (int i = 0; i < 4; i++)
					cout << BatteryInfo.Chemistry[i];
				cout << endl;
			}
			//}
		}

		LocalFree(DeviceInterfaceDetailData);
		SetupDiDestroyDeviceInfoList(DeviceInfoSet);
	}
}

void thread_func()
{

	while (input_char != 0)
	{
		system("cls");

		SYSTEM_POWER_STATUS SystemPowerStatus;
		GetSystemPowerStatus(&SystemPowerStatus);

		cout << "Type of power supply - ";
		switch (SystemPowerStatus.ACLineStatus)
		{
		case 0: cout << "Battery" << endl; break;

		case 1: cout << "AC" << endl; break;

		case 255: cout << "Unknown" << endl; break;

		}
		
		cout << "Information about type of battery - ";
		GetTypeBattery();

		cout << "Battery level - ";
		switch (SystemPowerStatus.BatteryLifePercent)
		{
		case 255: cout << "Unknown" << endl; break;

		default: cout << (int) SystemPowerStatus.BatteryLifePercent << "%" << endl; break;
		}

		cout << "Status of battery saver - ";
		switch (SystemPowerStatus.SystemStatusFlag)
		{
		case 0: cout << "Off" << endl; break;
		
		case 1: cout << "On" << endl; break;
		}

		cout << "Battery life without charging - ";
		if (SystemPowerStatus.BatteryLifeTime == -1)
			cout << "Unknown" << endl;
		else
		{
			unsigned long time = SystemPowerStatus.BatteryLifeTime;

			int hours = time / 3600;
			int min = (time - hours * 3600) / 60;
			int sec = time % 60; 

			cout << hours << " hours " << min << " minutes " << sec << " second" << endl;
		}

		cout << endl << "Press 0 - Exit, 1 - Sleep, 2 - Hibernate" << endl;

		Sleep(1000);
	}
}
