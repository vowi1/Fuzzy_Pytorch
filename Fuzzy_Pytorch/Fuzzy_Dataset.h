#pragma once
#include <vector>
#include <random>
#include "Fuzzy_Rule.h"
//
class Fuzzy_Dataset : public torch::data::datasets::Dataset<Fuzzy_Dataset>
{
	std::vector<std::vector<float>> vec_Inputs;
	std::vector<float> vec_Outputs;
public:
	Fuzzy_Dataset(int length_dataset)
	{
		int k;
		vec_Inputs.resize(length_dataset);
		vec_Outputs.resize(length_dataset);
		//
		Fuzzy_Rule FZ;
		//
		std::random_device rd;  // Will be used to obtain a seed for the random number engine
		std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
		std::uniform_real_distribution<float> price_rnd(15.0f, 50.0f);
		std::uniform_real_distribution<float> time_rnd(15.0f, 60.0f);
		std::uniform_real_distribution<float> quality_rnd(0.0f, 1.0f);
		// Price     15-50Euro
		// Time      15min-1h
		// Quality   0-1 
		float price, wtime, quality;
		for (k = 0; k < length_dataset; k++)
		{
			price = price_rnd(gen);
			wtime = time_rnd(gen);
			quality = quality_rnd(gen);

			std::vector<float> vec_in = FZ.normalize_Model_Input(price, wtime, quality);
			vec_Inputs[k] = vec_in;

			vec_Outputs[k] = FZ.apply_Rules(price, wtime, quality);
		}

	}
	torch::data::Example<> get(size_t index)
	{
		auto options1 = torch::TensorOptions().dtype(torch::kFloat32);
		//
		int le_input_vec = vec_Inputs[index].size();
		torch::Tensor tdata = torch::from_blob(vec_Inputs[index].data(), le_input_vec, torch::kFloat);//torch::zeros({ le_input_vec }, options1);
		//
		torch::Tensor tlabel = torch::tensor({ vec_Outputs[index] }, torch::kFloat);
		//
		return { tdata, tlabel };
	}
	torch::optional<size_t> size() const
	{
		return vec_Inputs.size();
	}
	//========================================================

};