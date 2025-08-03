#pragma once
#include <vector>
class Fuzzy_Rule
{
public:
	//
	float Min_Price = 15.0f;
	float Scale_Price = 50.0f;
	float Min_Time = 15.0f;
	float Scale_Time = 60.0f;
	float Scale_Quality = 1.0f;
	//
	std::vector<float> Scores_Show;
	std::vector<float> Scores_Pos;
	//=======================================
	std::vector<float> vec_Price;
	//
	std::vector<float> vec_Price_High;
	std::vector<float> vec_Price_Mid;
	std::vector<float> vec_Price_Low;
	//=======================================
	std::vector<float> vec_Time;
	//
	std::vector<float> vec_Time_High;
	std::vector<float> vec_Time_Mid;
	std::vector<float> vec_Time_Low;
	//=======================================
	std::vector<float> vec_Quality;
	//
	std::vector<float> vec_Quality_High;
	std::vector<float> vec_Quality_Mid;
	std::vector<float> vec_Quality_Low;
	//
	float last_price, last_time, last_quality;
	float last_result;
	//
	Fuzzy_Rule()
	{
		int k;
		int N = 100;
		//==========================================================
		float FZ_Price_High;
		float FZ_Price_Mid;
		float FZ_Price_Low;
		//
		vec_Price.resize(N);
		//
		vec_Price_High.resize(N);
		vec_Price_Mid.resize(N);
		vec_Price_Low.resize(N);
		//
		for (k = 0; k < N; k++)
		{
			vec_Price[k] = Min_Price+(Scale_Price- Min_Price) * k / static_cast<float>(N);
			calculate_Fuzzy_Membership_Triangle_Abs(vec_Price[k]-Min_Price, Scale_Price - Min_Price,
				FZ_Price_High,
				FZ_Price_Mid,
				FZ_Price_Low);
			//
			vec_Price_High[k] = FZ_Price_High;
			vec_Price_Mid[k]  = FZ_Price_Mid;
			vec_Price_Low[k]  = FZ_Price_Low;
		}
		//==========================================================
		float FZ_Time_High;
		float FZ_Time_Mid;
		float FZ_Time_Low;
		//
		vec_Time.resize(N);
		//
		vec_Time_High.resize(N);
		vec_Time_Mid.resize(N);
		vec_Time_Low.resize(N);
		//
		for (k = 0; k < N; k++)
		{
			vec_Time[k] = Min_Time+(Scale_Time - Min_Time) * k / static_cast<float>(N);
			calculate_Fuzzy_Membership_Triangle_Abs(vec_Time[k] - Min_Time, Scale_Time - Min_Time,
				FZ_Time_High,
				FZ_Time_Mid,
				FZ_Time_Low);
			//
			vec_Time_High[k] = FZ_Time_High;
			vec_Time_Mid[k]  = FZ_Time_Mid;
			vec_Time_Low[k]  = FZ_Time_Low;
		}
		//==========================================================
		float FZ_Quality_High;
		float FZ_Quality_Mid;
		float FZ_Quality_Low;
		//
		vec_Quality.resize(N);
		//
		vec_Quality_High.resize(N);
		vec_Quality_Mid.resize(N);
		vec_Quality_Low.resize(N);
		//
		for (k = 0; k < N; k++)
		{
			vec_Quality[k] = 1.0f * k / static_cast<float>(N);
			calculate_Fuzzy_Membership_Triangle_Abs(vec_Quality[k], 1.0f,
				FZ_Quality_High,
				FZ_Quality_Mid,
				FZ_Quality_Low);
			//
			vec_Quality_High[k] = FZ_Quality_High;
			vec_Quality_Mid[k]  = FZ_Quality_Mid;
			vec_Quality_Low[k]  = FZ_Quality_Low;
		}
		//==========================================================
	}
	std::vector<float> normalize_Model_Input(float price, float wtime, float quality)
	{
		std::vector<float> vec_in = { (price-0.5f*Scale_Price) / Scale_Price,(wtime-0.5f*Scale_Time) / Scale_Time,(quality-0.5f)/sqrt(12.0f)};

		return vec_in;
	}
	float apply_Rules(float price, float wtime,float quality)
	{
		last_price   = price;
		last_time   = wtime;
		last_quality = quality;
		//
		float FZ_Price_High;
		float FZ_Price_Mid;
		float FZ_Price_Low;
		calculate_Fuzzy_Membership_Triangle_Abs(price - Min_Price, Scale_Price - Min_Price,
			FZ_Price_High,
			FZ_Price_Mid,
			FZ_Price_Low);
		//
		float FZ_Time_High;
		float FZ_Time_Mid;
		float FZ_Time_Low;
		calculate_Fuzzy_Membership_Triangle_Abs(wtime-Min_Time, Scale_Time - Min_Time,
			FZ_Time_High,
			FZ_Time_Mid,
			FZ_Time_Low);
		//
		float FZ_Quality_High;
		float FZ_Quality_Mid;
		float FZ_Quality_Low;
		calculate_Fuzzy_Membership_Triangle_Abs(quality, 1.0f,
			FZ_Quality_High,
			FZ_Quality_Mid,
			FZ_Quality_Low);
		//Rules
		std::vector<float> Tip_VV_Low;
		std::vector<float> Tip_V_Low;
		std::vector<float> Tip_Low;
		std::vector<float> Tip_Mid;
		std::vector<float> Tip_High;
		std::vector<float> Tip_V_High;
		std::vector<float> Tip_VV_High;
		//
		Tip_VV_Low.push_back(std::min(FZ_Quality_Low, std::min(FZ_Price_High, FZ_Time_High)));
		Tip_V_Low.push_back(std::min(FZ_Quality_Low, std::min(FZ_Price_Mid, FZ_Time_High)));
		Tip_Low.push_back(std::min(FZ_Quality_Low, std::min(FZ_Price_Low, FZ_Time_High)));
		//
		Tip_V_Low.push_back(std::min(FZ_Quality_Low, std::min(FZ_Price_High, FZ_Time_Mid)));
		Tip_Low.push_back(std::min(FZ_Quality_Low, std::min(FZ_Price_Mid, FZ_Time_Mid)));
		Tip_Low.push_back(std::min(FZ_Quality_Low, std::min(FZ_Price_Low, FZ_Time_Mid)));
		//
		Tip_V_Low.push_back(std::min(FZ_Quality_Low, std::min(FZ_Price_High, FZ_Time_Low)));
		Tip_Low.push_back(std::min(FZ_Quality_Low, std::min(FZ_Price_Mid, FZ_Time_Low)));
		Tip_Low.push_back(std::min(FZ_Quality_Low, std::min(FZ_Price_Low, FZ_Time_Low)));
		//===================
		Tip_V_Low.push_back(std::min(FZ_Quality_Mid, std::min(FZ_Price_High, FZ_Time_High)));
		Tip_Low.push_back(std::min(FZ_Quality_Mid, std::min(FZ_Price_Mid, FZ_Time_High)));
		Tip_Mid.push_back(std::min(FZ_Quality_Mid, std::min(FZ_Price_Low, FZ_Time_High)));
		//
		Tip_Low.push_back(std::min(FZ_Quality_Mid, std::min(FZ_Price_High, FZ_Time_Mid)));
		Tip_Mid.push_back(std::min(FZ_Quality_Mid, std::min(FZ_Price_Mid, FZ_Time_Mid)));
		Tip_Mid.push_back(std::min(FZ_Quality_Mid, std::min(FZ_Price_Low, FZ_Time_Mid)));
		//
		Tip_Low.push_back(std::min(FZ_Quality_Mid, std::min(FZ_Price_High, FZ_Time_Low)));
		Tip_Mid.push_back(std::min(FZ_Quality_Mid, std::min(FZ_Price_Mid, FZ_Time_Low)));
		Tip_High.push_back(std::min(FZ_Quality_Mid, std::min(FZ_Price_Low, FZ_Time_Low)));
		//================
		Tip_Mid.push_back(std::min(FZ_Quality_High, std::min(FZ_Price_High, FZ_Time_High)));
		Tip_Mid.push_back(std::min(FZ_Quality_High, std::min(FZ_Price_Mid, FZ_Time_High)));
		Tip_High.push_back(std::min(FZ_Quality_High, std::min(FZ_Price_Low, FZ_Time_High)));
		//
		Tip_Mid.push_back(std::min(FZ_Quality_High, std::min(FZ_Price_High, FZ_Time_Mid)));
		Tip_High.push_back(std::min(FZ_Quality_High, std::min(FZ_Price_Mid, FZ_Time_Mid)));
		Tip_V_High.push_back(std::min(FZ_Quality_High, std::min(FZ_Price_Low, FZ_Time_Mid)));
		//
		Tip_High.push_back(std::min(FZ_Quality_High, std::min(FZ_Price_High, FZ_Time_Low)));
		Tip_V_High.push_back(std::min(FZ_Quality_High, std::min(FZ_Price_Mid, FZ_Time_Low)));
		Tip_VV_High.push_back(std::min(FZ_Quality_High, std::min(FZ_Price_Low, FZ_Time_Low)));
		//
		float Tip = defuzzify(Tip_VV_Low,
			Tip_V_Low,
			Tip_Low,
			Tip_Mid,
			Tip_High,
			Tip_V_High,
			Tip_VV_High);
		//
		last_result = Tip;
		return Tip;
	}
	void calculate_Fuzzy_Membership_Triangle_Abs(float FZ_Value, float FZ_Scale_High,
		float& FZ_High,
		float& FZ_Mid,
		float& FZ_Low)
	{
		float Ratio_FZ = fabs(FZ_Value / FZ_Scale_High);
		//===========================================================================
		// Vel Distance 
		float RL = 0.5;
		FZ_High = std::max(0.0f, std::min(1.0f, (Ratio_FZ - RL) / RL));
		if (Ratio_FZ > RL)
		{
			// Triangle Down
			FZ_Mid = std::max(0.0f, std::min(1.0f, 1.0f - (Ratio_FZ - RL) / RL));
		}
		else
		{
			// Triangle Up
			FZ_Mid = std::max(0.0f, std::min(1.0f, (Ratio_FZ - 0.0f) / RL));
		}
		FZ_Low = std::max(0.0f, std::min(1.0f, 1.0f - (Ratio_FZ - 0.0f) / RL));
		//
	}
	//==========================================================================
	float defuzzify(std::vector<float>& Tip_VV_Low, 
		std::vector<float>& Tip_V_Low,
		std::vector<float>& Tip_Low,
		std::vector<float>& Tip_Mid,
		std::vector<float>& Tip_High,
		std::vector<float>& Tip_V_High,
		std::vector<float>& Tip_VV_High)
	{
		int k;
		//
		std::vector<float> Scores(7, 0.0f);
		//
		if (Tip_VV_Low.size() > 0)
		{
			Scores[0] = *std::max_element(Tip_VV_Low.begin(), Tip_VV_Low.end());
		}
		if (Tip_V_Low.size() > 0)
		{
			Scores[1] = *std::max_element(Tip_V_Low.begin(), Tip_V_Low.end());
		}
		if (Tip_Low.size() > 0)
		{
			Scores[2] = *std::max_element(Tip_Low.begin(), Tip_Low.end());
		}
		if (Tip_Mid.size() > 0)
		{
			Scores[3] = *std::max_element(Tip_Mid.begin(), Tip_Mid.end());
		}
		if (Tip_High.size() > 0)
		{
			Scores[4] = *std::max_element(Tip_High.begin(), Tip_High.end());
		}
		if (Tip_V_High.size() > 0)
		{
			Scores[5] = *std::max_element(Tip_V_High.begin(), Tip_V_High.end());
		}
		if (Tip_VV_High.size() > 0)
		{
			Scores[6] = *std::max_element(Tip_VV_High.begin(), Tip_VV_High.end());
		}
		//===================================
		float Sum_Score = 0.0f;
		for (k = 0; k < Scores.size(); k++)
		{
			Sum_Score = Sum_Score + Scores[k];
		}
		//===================================
		if (Sum_Score < 1e-5)
		{
			Scores[0] = 1.0f;
		}
		float W_Frac = 1.0f / static_cast<float>(Scores.size() - 1);
		float Weighted_Average = 0.0f;
		std::vector<float> Diracs(Scores.size());
		for (k = 0; k < Scores.size(); k++)
		{
			Weighted_Average = Weighted_Average + static_cast<float>(k) * W_Frac * Scores[k] / Sum_Score;
			Diracs[k] = static_cast<float>(k) * W_Frac;
		}
		//= (S_VV_High*1.0 + S_MS_High * 0.75 + S_MS_Mid * 0.5 + S_MS_Low * 0.25 + S_MS_Very_Low * 0.0) / Sum;
		Scores_Show = Scores;
		Scores_Pos = Diracs;
		//
		return Weighted_Average;
	}
	//==========================================
};

