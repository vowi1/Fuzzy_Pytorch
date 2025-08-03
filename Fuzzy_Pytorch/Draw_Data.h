#pragma once
using namespace System;
using namespace System::Collections::Generic;
//
public ref class Draw_Data
{
public:
	List<float>^ Index;
	List<float>^ Loss;
	//
	List<float>^ Test_Index;
	List<float>^ Test_Loss;
	float Test_Error_Min;
	float Test_Error_Max;
public:
	Draw_Data()
	{
		Index = gcnew List<float>();
		Loss  = gcnew List<float>();
		//
		Test_Index = gcnew List<float>();
		Test_Loss = gcnew List<float>();
	}
	void clear()
	{
		Index->Clear();
		Loss->Clear();
	}
	void clear_test()
	{
		Test_Index->Clear();
		Test_Loss->Clear();
	}
};

