#pragma once
#pragma unmanaged
#include <torch.h>
#include <torch/script.h>
//
#include "Fuzzy_Dataset.h"

typedef enum {Activation_Relu,Activation_ELU,Activation_Leaky_Relu,Activation_Gelu,Activation_tanh,Activation_Sigmoid}Activation_Function;
//
typedef void(__stdcall* Native_Show_String_Callback)(std::string info, float Loss, int index, int epoch, int data_size);
//
// Define a new Module.
//==========================================================================
/*struct Net : torch::nn::Module
{
    Net() 
    {
        // Construct and register two Linear submodules.
        fc1 = register_module("fc1", torch::nn::Linear(784, 64));
        fc2 = register_module("fc2", torch::nn::Linear(64, 32));
        fc3 = register_module("fc3", torch::nn::Linear(32, 10));
    }

    // Implement the Net's algorithm.
    torch::Tensor forward(torch::Tensor x) 
    {
        // Use one of many tensor manipulation functions.
        x = torch::relu(fc1->forward(x.reshape({ x.size(0), 784 })));
        x = torch::dropout(x, 0.5, is_training());//p=
        x = torch::relu(fc2->forward(x));
        x = torch::log_softmax(fc3->forward(x), 1);//dim=
        return x;
    }

    // Use one of many "standard library" modules.
    torch::nn::Linear fc1{ nullptr }, fc2{ nullptr }, fc3{ nullptr };
};*/
//==========================================================================
struct NetworkImpl :public torch::nn::SequentialImpl
{
    NetworkImpl(int N_hidden,int N_Layer, Activation_Function Act)
    {
        using namespace torch::nn;
        push_back(Linear(3, N_hidden));
        add_Activation_Function(Act);
        //
        for (int k = 0; k < N_Layer; k++)
        {
            push_back(Linear(N_hidden, N_hidden));
            //push_back(Functional(torch::tanh));
            add_Activation_Function(Act);
        }
        //push_back(Linear(N_hidden, N_hidden));
        //push_back(Functional(torch::tanh));
        push_back(Linear(N_hidden, 1));
    }
    void add_Activation_Function(Activation_Function Act)
    {
        namespace F = torch::nn::functional;
        //
        switch (Act)
        {
        case Activation_Relu:
            push_back(torch::nn::Functional(torch::relu));
            break;
        case Activation_ELU:
            //auto opt = torch::nn::functional::ELUFuncOptions().alpha(0.42).inplace(true);
            push_back(torch::nn::Functional(
                [](torch::Tensor input) 
                { 
                    return torch::elu(input,1.0); 
                }));
            //push_back(torch::nn::Functional(torch::elu, ));
            break;
        case Activation_Leaky_Relu:
            //push_back(torch::nn::Functional(torch::leaky_relu));
            push_back(torch::nn::Functional(
                [](torch::Tensor input)
                {
                    return torch::leaky_relu(input, 0.01);
                }));
            break;
           
        case Activation_Gelu:
            //push_back(torch::nn::Functional(torch::gelu));
            push_back(torch::nn::Functional(
                [](torch::Tensor input)
                {
                    return torch::gelu(input, "tanh");
                    //return torch::gelu(input,  F::GELUFuncOptions());//.negative_slope(0.01).inplace(true));
                }));
            break;
        case Activation_tanh:
            push_back(torch::nn::Functional(torch::tanh));
            break;
        case Activation_Sigmoid:
            push_back(torch::nn::Functional(torch::sigmoid));
            break;
        }
    }
};
//TORCH_MODULE(Network);
//========================================================================
class Fuzzy_Imp
{
public:
    Fuzzy_Dataset* My_Train_Set = NULL;
    //Network network;
    std::shared_ptr<NetworkImpl> my_network;
    //
    int log_interval = 5;
    int train_batch_size = 16;
    int iterations = 1;
    float learning_rate = 0.001f;
    //
    bool abort_train = false;
    //
    Native_Show_String_Callback Show_Info_Callback;
    Native_Show_String_Callback Show_Test_Callback;
    //
    std::vector<std::string> str_network;
    //
    Fuzzy_Rule FZ;
public:
	Fuzzy_Imp(int N_hidden,int N_Layer, Activation_Function Act,
        Native_Show_String_Callback Show_Info_Callback_In,
        Native_Show_String_Callback Show_Test_Callback_In):
        Show_Info_Callback(Show_Info_Callback_In),
        Show_Test_Callback(Show_Test_Callback_In)
	{
        my_network = std::make_shared<NetworkImpl>(N_hidden, N_Layer,Act);
        //
        get_network_parameters();
	}
    void create_network(int N_hidden,int N_Layer, Activation_Function Act)
    {
        my_network = std::make_shared<NetworkImpl>(N_hidden,N_Layer,Act);
        get_network_parameters();
    }
    void save_net(std::string file_name)
    {
        torch::save(my_network,file_name);
    }
    void load_net(std::string file_name)
    {
        torch::load(my_network,file_name);
    }
    void get_network_parameters()
    {
        //str_network = "";
        //for (auto & par : my_network->parameters(true))
        //{
        //  //    str_network = str_network + "|" + par.name();
        //     par.type
        //}

        //std::cout << my_network->parameters();
        str_network.clear();
        //
        str_network.push_back("Network: " + my_network->name());
        for (auto & name : my_network->named_modules())
        {
            auto& m = name.value();
            const auto& t = m->named_parameters();
            
            const auto & k = t.keys();
            //
            std::stringstream my_stream;
            m->pretty_print(my_stream);
            std::string str_k = m->name() + " | " +  my_stream.str()+ " | ";// +t.key_description();
            for (auto& kk : k)
            {
                str_k = str_k + "," + kk;
            }
            str_network.push_back("Module: " + name.key()  + ", Keys: " + str_k);
        }
        int index = 0;
        int k;
        int N_Sum = 0;
        for (auto& pair : my_network->named_parameters(true))
        {
            auto& m = pair.value();
            //auto& d = pair.pair();
            int N_dim = m.dim();
            std::vector<int> dim(N_dim);
            std::string sz="";
            int N_par = 1;
            for (k = 0; k < N_dim; k++)
            {
                sz = sz + std::to_string(m.size(k));
                if (k < N_dim-1)
                {
                    sz = sz + ",";
                }
                N_par = N_par * m.size(k);
            }
            N_Sum = N_Sum+ N_par;
            str_network.push_back("Parameter: " + std::to_string(index) + " | " + pair.key() + ", N=" + std::to_string(N_dim) + " | " + sz + "| S=" +std::to_string(N_par));
            //
            index++;
        }
        str_network.push_back("Number of Parameters: " + std::to_string(N_Sum));
    }
    //=================================================
    template<typename DataLoader>
    void train(std::shared_ptr<NetworkImpl> & network,
        //Network& network,
        DataLoader& loader,
        torch::optim::Optimizer& optimizer,
        size_t epoch,
        size_t data_size)
    {
        size_t index = 0;
        network->train();
        float Loss = 0;
        int k;
        //
        for (auto& batch : loader)
        {
            auto data    = batch.data.to(torch::kCPU);
            auto targets = batch.target.to(torch::kCPU);
            //
            auto output = network->forward(data);
            auto o_dim = output.dim();
            auto o_targets = targets.dim();
            std::vector<int> N_out(o_dim);
            std::vector<int> N_targets(o_targets);
            for (k = 0; k < N_out.size(); k++)
            {
                N_out[k] = output.size(k);
            }
            for (k = 0; k < N_targets.size(); k++)
            {
                N_targets[k] = targets.size(k);//N Samples
            }
            //
            auto loss = torch::mse_loss(output, targets);

            assert(!std::isnan(loss.template item<float>()));
            //
            float current_loss = loss.item<float>();
            //
            std::vector<float> show_out(N_out[0]);
            std::vector<float> show_tar(N_out[0]);
            for (k = 0; k < N_out[0]; k++)
            {
                show_out[k] = output[0,k].item<float>();
                show_tar[k] = targets[0,k].item<float>();
            }
            //
            optimizer.zero_grad();
            loss.backward();
            optimizer.step();
            Loss += current_loss;
            //Loss = current_loss;
            //
            if (index++ % log_interval == 0)
            {
                auto end = std::min(data_size, (index + 1) * train_batch_size);

                std::string train_info = "Train Epoch: " + std::to_string(epoch) + " " + std::to_string(end) + "/" + std::to_string(data_size)
                    + "\tLoss: " + std::to_string(Loss / end) + "\t Current Loss: " + std::to_string(current_loss);
                //
                Show_Info_Callback(train_info,current_loss,index,epoch,data_size);
            }
            if (abort_train == true)
            {
                break;
            }
        }
    }
    //=============================================================
    //=================================================
    void train_main(int length_dataset)
    {
        torch::manual_seed(1);
        //
        My_Train_Set = new Fuzzy_Dataset(length_dataset);
        //
        auto train_set = My_Train_Set->map(torch::data::transforms::Stack<>());
        auto train_size = train_set.size().value();
        auto train_loader =
            torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
                std::move(train_set), train_batch_size);
        //================================
        my_network->to(torch::kCPU);

        torch::optim::Adam optimizer(
            my_network->parameters(), torch::optim::AdamOptions(learning_rate));

        //
        int k = 0;
        for (k = 0; k < iterations; k++)
        {
            train(my_network, *train_loader, optimizer, k + 1, train_size);
        }
        //
    }
    //=============================================================
    template<typename DataLoader>
    void test(std::shared_ptr<NetworkImpl>& network, 
        DataLoader& loader, size_t data_size)
    {
        size_t index = 0;
        network->eval();
        torch::NoGradGuard no_grad;
        //float Loss = 0;
        std::vector<float> Errors;
        for (const auto& batch : loader)
        {
            auto data = batch.data.to(torch::kCPU);
            auto targets = batch.target.to(torch::kCPU);

            auto output = network->forward(data);
            auto loss = torch::mse_loss(output, targets);
            assert(!std::isnan(loss.template item<float>()));

            //Loss += loss.template item<float>();
            //
            float current_loss = loss.item<float>();
            Errors.push_back(sqrtf(current_loss));
            std::vector<float> Errors_sorted = Errors;
            std::sort(Errors_sorted.begin(), Errors_sorted.end());
            float Median = Errors_sorted[Errors_sorted.size() / 2];
            //
            std::string test_info = "Test: " + std::to_string(index) +  ",  Median Error: " + std::to_string(Median);
            Show_Test_Callback(test_info, current_loss, index, 0, data_size);
            index++;
        }
    }
    //=================================================
    void test_main(int N_test)
    {
        My_Train_Set = new Fuzzy_Dataset(N_test);

        //auto train_set = CustomDataset(data.first,options,Image_Loader).map(torch::data::transforms::Stack<>());
        auto test_set = My_Train_Set->map(torch::data::transforms::Stack<>());
        auto test_size = test_set.size().value();
        auto test_loader =
            torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
                std::move(test_set), 1);
        //
        test(my_network, *test_loader, test_size);
        //================================
    }
    void test_model(float price, float wtime, float quality,float & FZ_Tip,float & Tip_Model)
    {
        my_network->eval();
        torch::NoGradGuard no_grad;
        //
        FZ_Tip = FZ.apply_Rules(price, wtime, quality);
        //
        std::vector<float> vec_in = FZ.normalize_Model_Input(price, wtime, quality); //{ price / FZ.Scale_Price, wtime / FZ.Scale_Time, quality };
        //
        torch::Tensor data = torch::from_blob(vec_in.data(), vec_in.size(), torch::kFloat);
        auto output = my_network->forward(data);
        //
        Tip_Model = output.item<float>();
    }
};

#pragma managed

