#pragma once
#include "Fuzzy_Imp.h"
#include "Draw_Data.h"
// uncomment to execute the rk1-utils:
//    #include "rk1_Utils_demo.h"  // shows how the rk1-utils can be used


namespace CppCLRWinFormsProject {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;
  using namespace System::Runtime::InteropServices;
  using namespace System::Threading;
  using namespace System::Collections::Generic;
  //
  public delegate System::Void Delegate_Show_String_Callback(std::string info,float Loss,int index,int epoch,int data_size);
  public delegate System::Void Delegate_Show_String_M_Callback(String^ info, float Loss, int index, int epoch, int data_size);
  public delegate System::Void Delegate_Invoke();
  /// <summary>
  /// Summary for Form1
  /// </summary>
  public ref class Form1 : public System::Windows::Forms::Form
  {
      Fuzzy_Imp* Test_Fuzzy;
      Draw_Data^ D_Data;
      Delegate_Show_String_Callback^ Del_Show_Info_Callback;
      Delegate_Show_String_Callback^ Del_Show_Test_Info_Callback;
      Thread^ p_Train;
      bool run_thread = false;
      NPlot::Windows::PlotSurface2D^ My_Plot_Surface;
      NPlot::Windows::PlotSurface2D^ Plot_Price;
      NPlot::Windows::PlotSurface2D^ Plot_Time;
      NPlot::Windows::PlotSurface2D^ Plot_Quality;
      NPlot::Windows::PlotSurface2D^ Plot_Fuzzy;
      //
  private: System::Windows::Forms::ListBox^ listBox_train_info;


  private: System::Windows::Forms::TrackBar^ trackBar_Price;
  private: System::Windows::Forms::TrackBar^ trackBar_Time;
  private: System::Windows::Forms::TrackBar^ trackBar_Quality;
  private: System::Windows::Forms::Label^ label_Price;
  private: System::Windows::Forms::Label^ label_Time;
  private: System::Windows::Forms::Button^ button_Abort_Train;
  private: System::Windows::Forms::Label^ label_Samples;
  private: System::Windows::Forms::NumericUpDown^ numericUpDown_Samples;
  private: System::Windows::Forms::GroupBox^ groupBox_Parameter;
  private: System::Windows::Forms::Label^ label_Batchsize;
  private: System::Windows::Forms::NumericUpDown^ numericUpDown_Batch_Size;
  private: System::Windows::Forms::NumericUpDown^ numericUpDown_Hidden_Nodes;
  private: System::Windows::Forms::Label^ label_Hidden;
  private: System::Windows::Forms::NumericUpDown^ numericUpDown_Learning_Rate;
  private: System::Windows::Forms::Label^ label_learning_rate;
  private: System::Windows::Forms::NumericUpDown^ numericUpDown_Layer;
  private: System::Windows::Forms::Label^ label1;
  private: System::Windows::Forms::Button^ button_Test;
  private: System::Windows::Forms::NumericUpDown^ numericUpDown_Test_Samples;
  private: System::Windows::Forms::Label^ label_Test_Samples;
  private: System::Windows::Forms::Label^ label_log_interval;
  private: System::Windows::Forms::NumericUpDown^ numericUpDown_Log_Intervall;
  private: System::Windows::Forms::Button^ button_Load;
  private: System::Windows::Forms::Button^ button_Save;
  private: System::Windows::Forms::FolderBrowserDialog^ folderBrowserDialog1;
  private: System::Windows::Forms::OpenFileDialog^ openFileDialog1;
  private: System::Windows::Forms::ComboBox^ comboBox_Act;
  private: System::Windows::Forms::TabControl^ tabControl1;
  private: System::Windows::Forms::TabPage^ tabPage_Train;
  private: System::Windows::Forms::TabPage^ tabPage_FZ;




  private: System::Windows::Forms::Label^ label_Quality;
         
  public:
    Form1(void)
    {
      InitializeComponent();
      //
      //TODO: Add the constructor code here
      //

      // uncomment to execute the rk1-utils:
      //    N_rk1_Utils_demo::execute(); // shows how the rk1-utils can be used
      Del_Show_Info_Callback = gcnew Delegate_Show_String_Callback(this, &Form1::show_train_info);
      Del_Show_Test_Info_Callback = gcnew Delegate_Show_String_Callback(this, &Form1::show_test_info);
      D_Data = gcnew Draw_Data();
      //
      this->comboBox_Act->SelectedIndexChanged -= gcnew System::EventHandler(this, &Form1::comboBox_Act_SelectedIndexChanged);
      comboBox_Act->SelectedIndex = 0;
      this->comboBox_Act->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::comboBox_Act_SelectedIndexChanged);
      Test_Fuzzy = new Fuzzy_Imp(Convert::ToInt32(numericUpDown_Hidden_Nodes->Value),
          Convert::ToInt32(numericUpDown_Layer->Value),
          get_current_Activation(),
          (Native_Show_String_Callback)Marshal::GetFunctionPointerForDelegate(Del_Show_Info_Callback).ToPointer(),
          (Native_Show_String_Callback)Marshal::GetFunctionPointerForDelegate(Del_Show_Test_Info_Callback).ToPointer());
      //
      int pos_x = 0;// listBox_train_info->Location.X;
      int pos_y = 0;// listBox_train_info->Location.Y + listBox_train_info->Height;
      int height, width;
      int x_off = 10;
      int y_off = 10;
      width = tabPage_Train->Width;// this->ClientSize.Height - pos_y - y_off;
      height = tabPage_Train->Height;// this->ClientSize.Width - pos_x - x_off;
      if (height < 10)
          height = 10;
      if (width < 10)
          width = 10;
      My_Plot_Surface = gcnew NPlot::Windows::PlotSurface2D();
      My_Plot_Surface->Location = System::Drawing::Point(pos_x, pos_y);
      My_Plot_Surface->Size = System::Drawing::Size(width, height);
      My_Plot_Surface->XAxis1 = nullptr;
      My_Plot_Surface->XAxis2 = nullptr;
      My_Plot_Surface->YAxis1 = nullptr;
      My_Plot_Surface->YAxis2 = nullptr;
      My_Plot_Surface->Legend = nullptr;
      My_Plot_Surface->AddInteraction(gcnew NPlot::Windows::PlotSurface2D::Interactions::RubberBandSelection());
      //this->Controls->Add(this->My_Plot_Surface);
      tabPage_Train->Controls->Add(this->My_Plot_Surface);
      My_Plot_Surface->SmoothingMode = System::Drawing::Drawing2D::SmoothingMode::HighSpeed;
      //===============================================
      Plot_Price = gcnew NPlot::Windows::PlotSurface2D();
      Plot_Price->Location = System::Drawing::Point(0, 0);
      Plot_Price->Size = System::Drawing::Size(width, height/4);
      Plot_Price->XAxis1 = nullptr;
      Plot_Price->XAxis2 = nullptr;
      Plot_Price->YAxis1 = nullptr;
      Plot_Price->YAxis2 = nullptr;
      Plot_Price->Legend = nullptr;
      Plot_Price->AddInteraction(gcnew NPlot::Windows::PlotSurface2D::Interactions::RubberBandSelection());
      //this->Controls->Add(this->My_Plot_Surface);
      tabPage_FZ->Controls->Add(this->Plot_Price);
      //===============================================
      Plot_Time = gcnew NPlot::Windows::PlotSurface2D();
      Plot_Time->Location = System::Drawing::Point(0, height / 4);
      Plot_Time->Size = System::Drawing::Size(width, height / 4);
      Plot_Time->XAxis1 = nullptr;
      Plot_Time->XAxis2 = nullptr;
      Plot_Time->YAxis1 = nullptr;
      Plot_Time->YAxis2 = nullptr;
      Plot_Time->Legend = nullptr;
      Plot_Time->AddInteraction(gcnew NPlot::Windows::PlotSurface2D::Interactions::RubberBandSelection());
      //this->Controls->Add(this->My_Plot_Surface);
      tabPage_FZ->Controls->Add(this->Plot_Time);
      //===============================================
      Plot_Quality = gcnew NPlot::Windows::PlotSurface2D();
      Plot_Quality->Location = System::Drawing::Point(0, height / 2);
      Plot_Quality->Size = System::Drawing::Size(width, height / 4);
      Plot_Quality->XAxis1 = nullptr;
      Plot_Quality->XAxis2 = nullptr;
      Plot_Quality->YAxis1 = nullptr;
      Plot_Quality->YAxis2 = nullptr;
      Plot_Quality->Legend = nullptr;
      Plot_Quality->AddInteraction(gcnew NPlot::Windows::PlotSurface2D::Interactions::RubberBandSelection());
      //this->Controls->Add(this->My_Plot_Surface);
      tabPage_FZ->Controls->Add(this->Plot_Quality);
      //===============================================
      Plot_Fuzzy = gcnew NPlot::Windows::PlotSurface2D();
      Plot_Fuzzy->Location = System::Drawing::Point(0, height*3 / 4);
      Plot_Fuzzy->Size = System::Drawing::Size(width, height / 4);
      Plot_Fuzzy->XAxis1 = nullptr;
      Plot_Fuzzy->XAxis2 = nullptr;
      Plot_Fuzzy->YAxis1 = nullptr;
      Plot_Fuzzy->YAxis2 = nullptr;
      Plot_Fuzzy->Legend = nullptr;
      Plot_Fuzzy->AddInteraction(gcnew NPlot::Windows::PlotSurface2D::Interactions::RubberBandSelection());
      //this->Controls->Add(this->My_Plot_Surface);
      tabPage_FZ->Controls->Add(this->Plot_Fuzzy);
      //===============================================
      this->Text = "Train Fuzzy-Rules: Model untrained";
      show_network();
    }

  protected:
    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    ~Form1()
    {
      if (components)
      {
        delete components;
      }
    }
  private: System::Windows::Forms::Button^ button_Train;
  protected:

  protected:





  protected:

  private:
    /// <summary>
    /// Required designer variable.
    /// </summary>
    System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    void InitializeComponent(void)
    {
        this->button_Train = (gcnew System::Windows::Forms::Button());
        this->listBox_train_info = (gcnew System::Windows::Forms::ListBox());
        this->trackBar_Price = (gcnew System::Windows::Forms::TrackBar());
        this->trackBar_Time = (gcnew System::Windows::Forms::TrackBar());
        this->trackBar_Quality = (gcnew System::Windows::Forms::TrackBar());
        this->label_Price = (gcnew System::Windows::Forms::Label());
        this->label_Time = (gcnew System::Windows::Forms::Label());
        this->label_Quality = (gcnew System::Windows::Forms::Label());
        this->button_Abort_Train = (gcnew System::Windows::Forms::Button());
        this->label_Samples = (gcnew System::Windows::Forms::Label());
        this->numericUpDown_Samples = (gcnew System::Windows::Forms::NumericUpDown());
        this->groupBox_Parameter = (gcnew System::Windows::Forms::GroupBox());
        this->comboBox_Act = (gcnew System::Windows::Forms::ComboBox());
        this->button_Load = (gcnew System::Windows::Forms::Button());
        this->button_Save = (gcnew System::Windows::Forms::Button());
        this->label_log_interval = (gcnew System::Windows::Forms::Label());
        this->numericUpDown_Log_Intervall = (gcnew System::Windows::Forms::NumericUpDown());
        this->numericUpDown_Layer = (gcnew System::Windows::Forms::NumericUpDown());
        this->label1 = (gcnew System::Windows::Forms::Label());
        this->label_learning_rate = (gcnew System::Windows::Forms::Label());
        this->numericUpDown_Learning_Rate = (gcnew System::Windows::Forms::NumericUpDown());
        this->numericUpDown_Hidden_Nodes = (gcnew System::Windows::Forms::NumericUpDown());
        this->label_Hidden = (gcnew System::Windows::Forms::Label());
        this->label_Batchsize = (gcnew System::Windows::Forms::Label());
        this->numericUpDown_Batch_Size = (gcnew System::Windows::Forms::NumericUpDown());
        this->button_Test = (gcnew System::Windows::Forms::Button());
        this->numericUpDown_Test_Samples = (gcnew System::Windows::Forms::NumericUpDown());
        this->label_Test_Samples = (gcnew System::Windows::Forms::Label());
        this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
        this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
        this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
        this->tabPage_Train = (gcnew System::Windows::Forms::TabPage());
        this->tabPage_FZ = (gcnew System::Windows::Forms::TabPage());
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_Price))->BeginInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_Time))->BeginInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_Quality))->BeginInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Samples))->BeginInit();
        this->groupBox_Parameter->SuspendLayout();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Log_Intervall))->BeginInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Layer))->BeginInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Learning_Rate))->BeginInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Hidden_Nodes))->BeginInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Batch_Size))->BeginInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Test_Samples))->BeginInit();
        this->tabControl1->SuspendLayout();
        this->SuspendLayout();
        // 
        // button_Train
        // 
        this->button_Train->Location = System::Drawing::Point(6, 6);
        this->button_Train->Name = L"button_Train";
        this->button_Train->Size = System::Drawing::Size(90, 41);
        this->button_Train->TabIndex = 0;
        this->button_Train->Text = L"Train";
        this->button_Train->UseVisualStyleBackColor = true;
        this->button_Train->Click += gcnew System::EventHandler(this, &Form1::button_Train_Click);
        // 
        // listBox_train_info
        // 
        this->listBox_train_info->FormattingEnabled = true;
        this->listBox_train_info->Location = System::Drawing::Point(6, 81);
        this->listBox_train_info->Name = L"listBox_train_info";
        this->listBox_train_info->Size = System::Drawing::Size(903, 225);
        this->listBox_train_info->TabIndex = 1;
        // 
        // trackBar_Price
        // 
        this->trackBar_Price->Location = System::Drawing::Point(285, 12);
        this->trackBar_Price->Maximum = 50;
        this->trackBar_Price->Minimum = 15;
        this->trackBar_Price->Name = L"trackBar_Price";
        this->trackBar_Price->Size = System::Drawing::Size(205, 45);
        this->trackBar_Price->TabIndex = 3;
        this->trackBar_Price->Value = 15;
        this->trackBar_Price->Scroll += gcnew System::EventHandler(this, &Form1::trackBar_Price_Scroll);
        // 
        // trackBar_Time
        // 
        this->trackBar_Time->Location = System::Drawing::Point(496, 12);
        this->trackBar_Time->Maximum = 60;
        this->trackBar_Time->Minimum = 15;
        this->trackBar_Time->Name = L"trackBar_Time";
        this->trackBar_Time->Size = System::Drawing::Size(191, 45);
        this->trackBar_Time->TabIndex = 4;
        this->trackBar_Time->Value = 15;
        this->trackBar_Time->Scroll += gcnew System::EventHandler(this, &Form1::trackBar_Time_Scroll);
        // 
        // trackBar_Quality
        // 
        this->trackBar_Quality->LargeChange = 10;
        this->trackBar_Quality->Location = System::Drawing::Point(693, 12);
        this->trackBar_Quality->Maximum = 100;
        this->trackBar_Quality->Name = L"trackBar_Quality";
        this->trackBar_Quality->Size = System::Drawing::Size(216, 45);
        this->trackBar_Quality->TabIndex = 5;
        this->trackBar_Quality->Value = 15;
        this->trackBar_Quality->Scroll += gcnew System::EventHandler(this, &Form1::trackBar_Quality_Scroll);
        // 
        // label_Price
        // 
        this->label_Price->AutoSize = true;
        this->label_Price->Location = System::Drawing::Point(351, 52);
        this->label_Price->Name = L"label_Price";
        this->label_Price->Size = System::Drawing::Size(31, 13);
        this->label_Price->TabIndex = 6;
        this->label_Price->Text = L"Price";
        // 
        // label_Time
        // 
        this->label_Time->AutoSize = true;
        this->label_Time->Location = System::Drawing::Point(568, 52);
        this->label_Time->Name = L"label_Time";
        this->label_Time->Size = System::Drawing::Size(30, 13);
        this->label_Time->TabIndex = 7;
        this->label_Time->Text = L"Time";
        // 
        // label_Quality
        // 
        this->label_Quality->AutoSize = true;
        this->label_Quality->Location = System::Drawing::Point(771, 52);
        this->label_Quality->Name = L"label_Quality";
        this->label_Quality->Size = System::Drawing::Size(39, 13);
        this->label_Quality->TabIndex = 8;
        this->label_Quality->Text = L"Quality";
        // 
        // button_Abort_Train
        // 
        this->button_Abort_Train->Location = System::Drawing::Point(98, 6);
        this->button_Abort_Train->Name = L"button_Abort_Train";
        this->button_Abort_Train->Size = System::Drawing::Size(90, 41);
        this->button_Abort_Train->TabIndex = 9;
        this->button_Abort_Train->Text = L"Abort";
        this->button_Abort_Train->UseVisualStyleBackColor = true;
        this->button_Abort_Train->Click += gcnew System::EventHandler(this, &Form1::button_Abort_Train_Click);
        // 
        // label_Samples
        // 
        this->label_Samples->AutoSize = true;
        this->label_Samples->Location = System::Drawing::Point(9, 57);
        this->label_Samples->Name = L"label_Samples";
        this->label_Samples->Size = System::Drawing::Size(77, 13);
        this->label_Samples->TabIndex = 10;
        this->label_Samples->Text = L"Train Samples:";
        // 
        // numericUpDown_Samples
        // 
        this->numericUpDown_Samples->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000, 0, 0, 0 });
        this->numericUpDown_Samples->Location = System::Drawing::Point(92, 55);
        this->numericUpDown_Samples->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 100000, 0, 0, 0 });
        this->numericUpDown_Samples->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000, 0, 0, 0 });
        this->numericUpDown_Samples->Name = L"numericUpDown_Samples";
        this->numericUpDown_Samples->Size = System::Drawing::Size(63, 20);
        this->numericUpDown_Samples->TabIndex = 11;
        this->numericUpDown_Samples->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10000, 0, 0, 0 });
        // 
        // groupBox_Parameter
        // 
        this->groupBox_Parameter->Controls->Add(this->comboBox_Act);
        this->groupBox_Parameter->Controls->Add(this->button_Load);
        this->groupBox_Parameter->Controls->Add(this->button_Save);
        this->groupBox_Parameter->Controls->Add(this->label_log_interval);
        this->groupBox_Parameter->Controls->Add(this->numericUpDown_Log_Intervall);
        this->groupBox_Parameter->Controls->Add(this->numericUpDown_Layer);
        this->groupBox_Parameter->Controls->Add(this->label1);
        this->groupBox_Parameter->Controls->Add(this->label_learning_rate);
        this->groupBox_Parameter->Controls->Add(this->numericUpDown_Learning_Rate);
        this->groupBox_Parameter->Controls->Add(this->numericUpDown_Hidden_Nodes);
        this->groupBox_Parameter->Controls->Add(this->label_Hidden);
        this->groupBox_Parameter->Controls->Add(this->label_Batchsize);
        this->groupBox_Parameter->Controls->Add(this->numericUpDown_Batch_Size);
        this->groupBox_Parameter->Location = System::Drawing::Point(930, 12);
        this->groupBox_Parameter->Name = L"groupBox_Parameter";
        this->groupBox_Parameter->Size = System::Drawing::Size(221, 293);
        this->groupBox_Parameter->TabIndex = 12;
        this->groupBox_Parameter->TabStop = false;
        this->groupBox_Parameter->Text = L"Parameter";
        // 
        // comboBox_Act
        // 
        this->comboBox_Act->FormattingEnabled = true;
        this->comboBox_Act->Items->AddRange(gcnew cli::array< System::Object^  >(6) {
            L"Activation_Relu", L"Activation_ELU", L"Activation_Leaky_Relu",
                L"Activation_Gelu", L"Activation_tanh", L"Activation_Sigmoid"
        });
        this->comboBox_Act->Location = System::Drawing::Point(18, 177);
        this->comboBox_Act->Name = L"comboBox_Act";
        this->comboBox_Act->Size = System::Drawing::Size(170, 21);
        this->comboBox_Act->TabIndex = 18;
        this->comboBox_Act->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::comboBox_Act_SelectedIndexChanged);
        // 
        // button_Load
        // 
        this->button_Load->Location = System::Drawing::Point(116, 246);
        this->button_Load->Name = L"button_Load";
        this->button_Load->Size = System::Drawing::Size(98, 41);
        this->button_Load->TabIndex = 17;
        this->button_Load->Text = L"Load";
        this->button_Load->UseVisualStyleBackColor = true;
        this->button_Load->Click += gcnew System::EventHandler(this, &Form1::button_Load_Click);
        // 
        // button_Save
        // 
        this->button_Save->Location = System::Drawing::Point(6, 246);
        this->button_Save->Name = L"button_Save";
        this->button_Save->Size = System::Drawing::Size(104, 41);
        this->button_Save->TabIndex = 16;
        this->button_Save->Text = L"Save";
        this->button_Save->UseVisualStyleBackColor = true;
        this->button_Save->Click += gcnew System::EventHandler(this, &Form1::button_Save_Click);
        // 
        // label_log_interval
        // 
        this->label_log_interval->AutoSize = true;
        this->label_log_interval->Location = System::Drawing::Point(22, 80);
        this->label_log_interval->Name = L"label_log_interval";
        this->label_log_interval->Size = System::Drawing::Size(63, 13);
        this->label_log_interval->TabIndex = 9;
        this->label_log_interval->Text = L"Log Interval";
        // 
        // numericUpDown_Log_Intervall
        // 
        this->numericUpDown_Log_Intervall->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
        this->numericUpDown_Log_Intervall->Location = System::Drawing::Point(88, 77);
        this->numericUpDown_Log_Intervall->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000, 0, 0, 0 });
        this->numericUpDown_Log_Intervall->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
        this->numericUpDown_Log_Intervall->Name = L"numericUpDown_Log_Intervall";
        this->numericUpDown_Log_Intervall->Size = System::Drawing::Size(67, 20);
        this->numericUpDown_Log_Intervall->TabIndex = 8;
        this->numericUpDown_Log_Intervall->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 5, 0, 0, 0 });
        this->numericUpDown_Log_Intervall->ValueChanged += gcnew System::EventHandler(this, &Form1::numericUpDown_Log_Intervall_ValueChanged);
        // 
        // numericUpDown_Layer
        // 
        this->numericUpDown_Layer->Location = System::Drawing::Point(88, 145);
        this->numericUpDown_Layer->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
        this->numericUpDown_Layer->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
        this->numericUpDown_Layer->Name = L"numericUpDown_Layer";
        this->numericUpDown_Layer->Size = System::Drawing::Size(56, 20);
        this->numericUpDown_Layer->TabIndex = 7;
        this->numericUpDown_Layer->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
        this->numericUpDown_Layer->ValueChanged += gcnew System::EventHandler(this, &Form1::numericUpDown_Layer_ValueChanged);
        // 
        // label1
        // 
        this->label1->AutoSize = true;
        this->label1->Location = System::Drawing::Point(43, 149);
        this->label1->Name = L"label1";
        this->label1->Size = System::Drawing::Size(33, 13);
        this->label1->TabIndex = 6;
        this->label1->Text = L"Layer";
        // 
        // label_learning_rate
        // 
        this->label_learning_rate->AutoSize = true;
        this->label_learning_rate->Location = System::Drawing::Point(10, 54);
        this->label_learning_rate->Name = L"label_learning_rate";
        this->label_learning_rate->Size = System::Drawing::Size(74, 13);
        this->label_learning_rate->TabIndex = 5;
        this->label_learning_rate->Text = L"Learning Rate";
        // 
        // numericUpDown_Learning_Rate
        // 
        this->numericUpDown_Learning_Rate->DecimalPlaces = 5;
        this->numericUpDown_Learning_Rate->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 327680 });
        this->numericUpDown_Learning_Rate->Location = System::Drawing::Point(88, 51);
        this->numericUpDown_Learning_Rate->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 131072 });
        this->numericUpDown_Learning_Rate->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 327680 });
        this->numericUpDown_Learning_Rate->Name = L"numericUpDown_Learning_Rate";
        this->numericUpDown_Learning_Rate->Size = System::Drawing::Size(67, 20);
        this->numericUpDown_Learning_Rate->TabIndex = 4;
        this->numericUpDown_Learning_Rate->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 196608 });
        this->numericUpDown_Learning_Rate->ValueChanged += gcnew System::EventHandler(this, &Form1::numericUpDown_Learning_Rate_ValueChanged);
        // 
        // numericUpDown_Hidden_Nodes
        // 
        this->numericUpDown_Hidden_Nodes->Location = System::Drawing::Point(88, 119);
        this->numericUpDown_Hidden_Nodes->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3, 0, 0, 0 });
        this->numericUpDown_Hidden_Nodes->Name = L"numericUpDown_Hidden_Nodes";
        this->numericUpDown_Hidden_Nodes->Size = System::Drawing::Size(56, 20);
        this->numericUpDown_Hidden_Nodes->TabIndex = 3;
        this->numericUpDown_Hidden_Nodes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 9, 0, 0, 0 });
        this->numericUpDown_Hidden_Nodes->ValueChanged += gcnew System::EventHandler(this, &Form1::numericUpDown_Hidden_Nodes_ValueChanged);
        // 
        // label_Hidden
        // 
        this->label_Hidden->AutoSize = true;
        this->label_Hidden->Location = System::Drawing::Point(43, 123);
        this->label_Hidden->Name = L"label_Hidden";
        this->label_Hidden->Size = System::Drawing::Size(41, 13);
        this->label_Hidden->TabIndex = 2;
        this->label_Hidden->Text = L"Hidden";
        // 
        // label_Batchsize
        // 
        this->label_Batchsize->AutoSize = true;
        this->label_Batchsize->Location = System::Drawing::Point(31, 28);
        this->label_Batchsize->Name = L"label_Batchsize";
        this->label_Batchsize->Size = System::Drawing::Size(53, 13);
        this->label_Batchsize->TabIndex = 1;
        this->label_Batchsize->Text = L"Batchsize";
        // 
        // numericUpDown_Batch_Size
        // 
        this->numericUpDown_Batch_Size->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
        this->numericUpDown_Batch_Size->Location = System::Drawing::Point(88, 25);
        this->numericUpDown_Batch_Size->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
        this->numericUpDown_Batch_Size->Name = L"numericUpDown_Batch_Size";
        this->numericUpDown_Batch_Size->Size = System::Drawing::Size(67, 20);
        this->numericUpDown_Batch_Size->TabIndex = 0;
        this->numericUpDown_Batch_Size->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 16, 0, 0, 0 });
        this->numericUpDown_Batch_Size->ValueChanged += gcnew System::EventHandler(this, &Form1::numericUpDown_Batch_Size_ValueChanged);
        // 
        // button_Test
        // 
        this->button_Test->Location = System::Drawing::Point(189, 6);
        this->button_Test->Name = L"button_Test";
        this->button_Test->Size = System::Drawing::Size(90, 41);
        this->button_Test->TabIndex = 13;
        this->button_Test->Text = L"Test";
        this->button_Test->UseVisualStyleBackColor = true;
        this->button_Test->Click += gcnew System::EventHandler(this, &Form1::button_Test_Click);
        // 
        // numericUpDown_Test_Samples
        // 
        this->numericUpDown_Test_Samples->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
        this->numericUpDown_Test_Samples->Location = System::Drawing::Point(244, 55);
        this->numericUpDown_Test_Samples->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000, 0, 0, 0 });
        this->numericUpDown_Test_Samples->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
        this->numericUpDown_Test_Samples->Name = L"numericUpDown_Test_Samples";
        this->numericUpDown_Test_Samples->Size = System::Drawing::Size(63, 20);
        this->numericUpDown_Test_Samples->TabIndex = 15;
        this->numericUpDown_Test_Samples->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 100, 0, 0, 0 });
        // 
        // label_Test_Samples
        // 
        this->label_Test_Samples->AutoSize = true;
        this->label_Test_Samples->Location = System::Drawing::Point(161, 57);
        this->label_Test_Samples->Name = L"label_Test_Samples";
        this->label_Test_Samples->Size = System::Drawing::Size(74, 13);
        this->label_Test_Samples->TabIndex = 14;
        this->label_Test_Samples->Text = L"Test Samples:";
        // 
        // openFileDialog1
        // 
        this->openFileDialog1->FileName = L"openFileDialog1";
        // 
        // tabControl1
        // 
        this->tabControl1->Controls->Add(this->tabPage_Train);
        this->tabControl1->Controls->Add(this->tabPage_FZ);
        this->tabControl1->Location = System::Drawing::Point(6, 312);
        this->tabControl1->Name = L"tabControl1";
        this->tabControl1->SelectedIndex = 0;
        this->tabControl1->Size = System::Drawing::Size(1145, 491);
        this->tabControl1->TabIndex = 16;
        // 
        // tabPage_Train
        // 
        this->tabPage_Train->Location = System::Drawing::Point(4, 22);
        this->tabPage_Train->Name = L"tabPage_Train";
        this->tabPage_Train->Padding = System::Windows::Forms::Padding(3);
        this->tabPage_Train->Size = System::Drawing::Size(1137, 465);
        this->tabPage_Train->TabIndex = 0;
        this->tabPage_Train->Text = L"Train/Test";
        this->tabPage_Train->UseVisualStyleBackColor = true;
        // 
        // tabPage_FZ
        // 
        this->tabPage_FZ->Location = System::Drawing::Point(4, 22);
        this->tabPage_FZ->Name = L"tabPage_FZ";
        this->tabPage_FZ->Padding = System::Windows::Forms::Padding(3);
        this->tabPage_FZ->Size = System::Drawing::Size(1137, 465);
        this->tabPage_FZ->TabIndex = 1;
        this->tabPage_FZ->Text = L"Fuzzy Functions";
        this->tabPage_FZ->UseVisualStyleBackColor = true;
        // 
        // Form1
        // 
        this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
        this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
        this->ClientSize = System::Drawing::Size(1156, 810);
        this->Controls->Add(this->tabControl1);
        this->Controls->Add(this->numericUpDown_Test_Samples);
        this->Controls->Add(this->label_Test_Samples);
        this->Controls->Add(this->button_Test);
        this->Controls->Add(this->groupBox_Parameter);
        this->Controls->Add(this->numericUpDown_Samples);
        this->Controls->Add(this->label_Samples);
        this->Controls->Add(this->button_Abort_Train);
        this->Controls->Add(this->label_Quality);
        this->Controls->Add(this->label_Time);
        this->Controls->Add(this->label_Price);
        this->Controls->Add(this->trackBar_Quality);
        this->Controls->Add(this->trackBar_Time);
        this->Controls->Add(this->trackBar_Price);
        this->Controls->Add(this->listBox_train_info);
        this->Controls->Add(this->button_Train);
        this->Name = L"Form1";
        this->Text = L"Form1";
        this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_Price))->EndInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_Time))->EndInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_Quality))->EndInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Samples))->EndInit();
        this->groupBox_Parameter->ResumeLayout(false);
        this->groupBox_Parameter->PerformLayout();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Log_Intervall))->EndInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Layer))->EndInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Learning_Rate))->EndInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Hidden_Nodes))->EndInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Batch_Size))->EndInit();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown_Test_Samples))->EndInit();
        this->tabControl1->ResumeLayout(false);
        this->ResumeLayout(false);
        this->PerformLayout();

    }
#pragma endregion

   // You can call the functions at a button click. If you prefer, 
    // you can call them by clicking a menu item.


    public: System::Void show_test_info(std::string info, float Loss, int index, int epoch, int data_size)
    {
        Monitor::Enter(D_Data);
        
            array<Object^>^ myArray = gcnew array<Object^>(5);
            String^ m_info = gcnew String(info.c_str());
            //
            myArray[0] = m_info;
            myArray[1] = Loss;
            myArray[2] = index;
            myArray[3] = epoch;
            myArray[4] = data_size;
            this->BeginInvoke(gcnew Delegate_Show_String_M_Callback(this, &Form1::draw_Test_Info), myArray);
       Monitor::Exit(D_Data);
        
    }
          public: System::Void draw_Test_Info(String^ info, float Loss, int index, int epoch, int data_size)
          {
              Monitor::Enter(D_Data);
              //
              float Error = sqrtf(Loss);
              //
              D_Data->Test_Loss->Add(Error);
              D_Data->Test_Index->Add(D_Data->Test_Loss->Count);
              //
              listBox_train_info->BeginUpdate();
              listBox_train_info->Items->Add(info);
              int pos = listBox_train_info->Items->Count - 1;
              listBox_train_info->SelectedIndex = pos;
              listBox_train_info->EndUpdate();
              //
              if (D_Data->Test_Loss->Count == 1)
              {
                  D_Data->Test_Error_Min = Error;
                  D_Data->Test_Error_Max = Error;
              }
              else
              {
                  D_Data->Test_Error_Min = System::Math::Min(Error, D_Data->Test_Error_Min);
                  D_Data->Test_Error_Max = System::Math::Max(Error, D_Data->Test_Error_Max);
              }
              My_Plot_Surface->Clear();
              //==============================================
              NPlot::LinePlot^ lp = gcnew NPlot::LinePlot();
              //
              lp->OrdinateData = D_Data->Test_Loss->ToArray();
              lp->AbscissaData = D_Data->Test_Index->ToArray();
              lp->Pen = gcnew Pen(Color::Green);
              lp->Label = "Index: " + index.ToString();
              //
              My_Plot_Surface->Add(lp);
              //
              My_Plot_Surface->XAxis1->Label = "Index, Min: " + D_Data->Test_Error_Min.ToString("n2") + ", Max: "  + D_Data->Test_Error_Max.ToString("n2");
              My_Plot_Surface->YAxis1->Label = "MSE-Loss";
              //
              My_Plot_Surface->Refresh();
              //==============================================
              Monitor::Exit(D_Data);
          }
public: System::Void draw_Fuzzy()
{
    //============================================== 
    array<float>^ vec_Price = gcnew array<float>(Test_Fuzzy->FZ.vec_Price.size());
    array<float>^ vec_Price_High = gcnew array<float>(Test_Fuzzy->FZ.vec_Price.size());
    array<float>^ vec_Price_Mid = gcnew array<float>(Test_Fuzzy->FZ.vec_Price.size());
    array<float>^ vec_Price_Low = gcnew array<float>(Test_Fuzzy->FZ.vec_Price.size());
    //
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.vec_Price.data(), vec_Price,0,vec_Price->Length);
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.vec_Price_High.data(), vec_Price_High, 0, vec_Price->Length);
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.vec_Price_Mid.data(), vec_Price_Mid, 0, vec_Price->Length);
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.vec_Price_Low.data(), vec_Price_Low, 0, vec_Price->Length);
    //
    Plot_Price->Clear();
    NPlot::LinePlot^ lp_High = gcnew NPlot::LinePlot();
    //
    lp_High->OrdinateData = vec_Price_High;
    lp_High->AbscissaData = vec_Price;
    lp_High->Pen = gcnew Pen(Color::Red);
    lp_High->Label = "High";
    Plot_Price->Add(lp_High);
    //
    NPlot::LinePlot^ lp_Mid = gcnew NPlot::LinePlot();
    //
    lp_Mid->OrdinateData = vec_Price_Mid;
    lp_Mid->AbscissaData = vec_Price;
    lp_Mid->Pen = gcnew Pen(Color::Green);
    lp_Mid->Label = "Mid";
    Plot_Price->Add(lp_Mid);
    //
    NPlot::LinePlot^ lp_Low = gcnew NPlot::LinePlot();
    //
    lp_Low->OrdinateData = vec_Price_Low;
    lp_Low->AbscissaData = vec_Price;
    lp_Low->Pen = gcnew Pen(Color::Blue);
    lp_Low->Label = "Low";
    Plot_Price->Add(lp_Low);
    //====================
    NPlot::LinePlot^ lp_Bar1 = gcnew NPlot::LinePlot();
    //
    lp_Bar1->OrdinateData = gcnew array<float>{0.0f, 1.0f};
    lp_Bar1->AbscissaData = gcnew array<float>{Test_Fuzzy->FZ.last_price, Test_Fuzzy->FZ.last_price };
    lp_Bar1->Pen = gcnew Pen(Color::Magenta, 2.0f);
    //
    Plot_Price->Add(lp_Bar1);
    //=========================
    //
    Plot_Price->XAxis1->Label = "Price";
    Plot_Price->YAxis1->Label = "Fuzzy";
    //
    Plot_Price->Refresh();
    //==============================================
     //============================================== 
    array<float>^ vec_Time = gcnew array<float>(Test_Fuzzy->FZ.vec_Time.size());
    array<float>^ vec_Time_High = gcnew array<float>(Test_Fuzzy->FZ.vec_Time.size());
    array<float>^ vec_Time_Mid = gcnew array<float>(Test_Fuzzy->FZ.vec_Time.size());
    array<float>^ vec_Time_Low = gcnew array<float>(Test_Fuzzy->FZ.vec_Time.size());
    //
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.vec_Time.data(), vec_Time, 0, vec_Time->Length);
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.vec_Time_High.data(), vec_Time_High, 0, vec_Time->Length);
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.vec_Time_Mid.data(), vec_Time_Mid, 0, vec_Time->Length);
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.vec_Time_Low.data(), vec_Time_Low, 0, vec_Time->Length);
    //
    Plot_Time->Clear();
    NPlot::LinePlot^ lp_Time_High = gcnew NPlot::LinePlot();
    //
    lp_Time_High->OrdinateData = vec_Time_High;
    lp_Time_High->AbscissaData = vec_Time;
    lp_Time_High->Pen = gcnew Pen(Color::Red);
    lp_Time_High->Label = "High";
    Plot_Time->Add(lp_Time_High);
    //
    NPlot::LinePlot^ lp_Time_Mid = gcnew NPlot::LinePlot();
    //
    lp_Time_Mid->OrdinateData = vec_Time_Mid;
    lp_Time_Mid->AbscissaData = vec_Time;
    lp_Time_Mid->Pen = gcnew Pen(Color::Green);
    lp_Time_Mid->Label = "Mid";
    Plot_Time->Add(lp_Time_Mid);
    //
    NPlot::LinePlot^ lp_Time_Low = gcnew NPlot::LinePlot();
    //
    lp_Time_Low->OrdinateData = vec_Time_Low;
    lp_Time_Low->AbscissaData = vec_Time;
    lp_Time_Low->Pen = gcnew Pen(Color::Blue);
    lp_Time_Low->Label = "Low";
    Plot_Time->Add(lp_Time_Low);
    //====================
    NPlot::LinePlot^ lp_Bar2 = gcnew NPlot::LinePlot();
    //
    lp_Bar2->OrdinateData = gcnew array<float>{0.0f, 1.0f};
    lp_Bar2->AbscissaData = gcnew array<float>{Test_Fuzzy->FZ.last_time, Test_Fuzzy->FZ.last_time };
    lp_Bar2->Pen = gcnew Pen(Color::Magenta, 2.0f);
    //
    Plot_Time->Add(lp_Bar2);
    //=========================
    //
    Plot_Time->XAxis1->Label = "Time";
    Plot_Time->YAxis1->Label = "Fuzzy";
    //
    Plot_Time->Refresh();
    //============================================== 
    array<float>^ vec_Qu = gcnew array<float>(Test_Fuzzy->FZ.vec_Quality.size());
    array<float>^ vec_Qu_High = gcnew array<float>(Test_Fuzzy->FZ.vec_Quality.size());
    array<float>^ vec_Qu_Mid = gcnew array<float>(Test_Fuzzy->FZ.vec_Quality.size());
    array<float>^ vec_Qu_Low = gcnew array<float>(Test_Fuzzy->FZ.vec_Quality.size());
    //
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.vec_Quality.data(), vec_Qu, 0, vec_Qu->Length);
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.vec_Quality_High.data(), vec_Qu_High, 0, vec_Qu->Length);
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.vec_Quality_Mid.data(), vec_Qu_Mid, 0, vec_Qu->Length);
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.vec_Quality_Low.data(), vec_Qu_Low, 0, vec_Qu->Length);
    //
    Plot_Quality->Clear();
    NPlot::LinePlot^ lp_Qu_High = gcnew NPlot::LinePlot();
    //
    lp_Qu_High->OrdinateData = vec_Qu_High;
    lp_Qu_High->AbscissaData = vec_Qu;
    lp_Qu_High->Pen = gcnew Pen(Color::Red);
    lp_Qu_High->Label = "High";
    Plot_Quality->Add(lp_Qu_High);
    //
    NPlot::LinePlot^ lp_Qu_Mid = gcnew NPlot::LinePlot();
    //
    lp_Qu_Mid->OrdinateData = vec_Qu_Mid;
    lp_Qu_Mid->AbscissaData = vec_Qu;
    lp_Qu_Mid->Pen = gcnew Pen(Color::Green);
    lp_Qu_Mid->Label = "Mid";
    Plot_Quality->Add(lp_Qu_Mid);
    //
    NPlot::LinePlot^ lp_Qu_Low = gcnew NPlot::LinePlot();
    //
    lp_Qu_Low->OrdinateData = vec_Qu_Low;
    lp_Qu_Low->AbscissaData = vec_Qu;
    lp_Qu_Low->Pen = gcnew Pen(Color::Blue);
    lp_Qu_Low->Label = "Low";
    Plot_Quality->Add(lp_Qu_Low);
    //====================
    NPlot::LinePlot^ lp_Bar3 = gcnew NPlot::LinePlot();
    //
    lp_Bar3->OrdinateData = gcnew array<float>{0.0f, 1.0f};
    lp_Bar3->AbscissaData = gcnew array<float>{Test_Fuzzy->FZ.last_quality, Test_Fuzzy->FZ.last_quality };
    lp_Bar3->Pen = gcnew Pen(Color::Magenta, 2.0f);
    //
    Plot_Quality->Add(lp_Bar3);
    //======================================
    Plot_Quality->XAxis1->Label = "Quality";
    Plot_Quality->YAxis1->Label = "Fuzzy";
    //
    Plot_Quality->Refresh();
    //=======================================
    Plot_Fuzzy->Clear();
    array<float>^ vec_FZ = gcnew  array<float>(Test_Fuzzy->FZ.Scores_Pos.size());
    array<float>^ Scores_FZ = gcnew  array<float>(Test_Fuzzy->FZ.Scores_Pos.size());
    //
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.Scores_Pos.data(), vec_FZ, 0, vec_FZ->Length);
    Marshal::Copy((IntPtr)Test_Fuzzy->FZ.Scores_Show.data(), Scores_FZ, 0, Scores_FZ->Length);
    //
    NPlot::PointPlot^ pp = gcnew NPlot::PointPlot();
    pp->OrdinateData = Scores_FZ;
    pp->AbscissaData = vec_FZ;
    pp->Marker = gcnew NPlot::Marker(NPlot::Marker::MarkerType::Cross2, 6, gcnew Pen(Color::Green));
    for (int k = 0; k < vec_FZ->Length; k++)
    {
        NPlot::LinePlot^ lp_Score = gcnew NPlot::LinePlot();
        //
        lp_Score->OrdinateData = gcnew array<float>{0.0f, Scores_FZ[k]};
        lp_Score->AbscissaData = gcnew array<float>{vec_FZ[k], vec_FZ[k] };
        lp_Score->Pen = gcnew Pen(Color::Blue,2.0f);
        //lp_Score->Label = "Low";
        Plot_Fuzzy->Add(lp_Score);
    }
    Plot_Fuzzy->XAxis1->Label = "Singletons " + Test_Fuzzy->FZ.last_result.ToString("n2");
    Plot_Fuzzy->YAxis1->Label = "Fuzzy";
    //
    Plot_Fuzzy->Add(pp);
    Plot_Fuzzy->Refresh();
}
  public: System::Void show_train_info(std::string info, float Loss, int index, int epoch, int data_size)
  {
      if (Monitor::TryEnter(D_Data) == true)
      {
          array<Object^>^ myArray = gcnew array<Object^>(5);
          String^ m_info = gcnew String(info.c_str());
          //
          myArray[0] = m_info;
          myArray[1] = Loss;
          myArray[2] = index;
          myArray[3] = epoch;
          myArray[4] = data_size;
          this->BeginInvoke(gcnew Delegate_Show_String_M_Callback(this, &Form1::draw_Info), myArray);
          Monitor::Exit(D_Data);
      }
  }
  public: System::Void draw_Info(String^ info, float Loss, int index, int epoch, int data_size)
  {
      Monitor::Enter(D_Data);
      //
      D_Data->Loss->Add(Loss);
      D_Data->Index->Add(D_Data->Loss->Count);
      //
      listBox_train_info->BeginUpdate();
       listBox_train_info->Items->Add(info);
       int pos = listBox_train_info->Items->Count - 1;
       listBox_train_info->SelectedIndex = pos;
      listBox_train_info->EndUpdate();
      //
      My_Plot_Surface->Clear();
       //==============================================
      NPlot::LinePlot^ lp = gcnew NPlot::LinePlot();
      //
      lp->OrdinateData = D_Data->Loss->ToArray();
      lp->AbscissaData = D_Data->Index->ToArray();
      lp->Pen = gcnew Pen(Color::Red);
      lp->Label = "Index: " + index.ToString();
      //
      My_Plot_Surface->Add(lp);
      //
      My_Plot_Surface->XAxis1->Label = "Index";
      My_Plot_Surface->YAxis1->Label = "MSE-Loss";
      //
      My_Plot_Surface->Refresh();
      //==============================================
      Monitor::Exit(D_Data);
  }
  private: System::Void button_Train_Click(System::Object^ sender, System::EventArgs^ e) 
  {
      if (run_thread == false)
      {
          listBox_train_info->Items->Clear();
          D_Data->clear();
          p_Train = gcnew Thread(gcnew ThreadStart(this, &Form1::train));
          p_Train->Start();
          button_Train->Enabled = false;
          button_Abort_Train->Enabled = true;
          groupBox_Parameter->Enabled = false;
      }
  }
  private: System::Void train()
   {
      run_thread = true;
      Test_Fuzzy->train_main(Convert::ToInt32(numericUpDown_Samples->Value));
      run_thread = false;
      this->BeginInvoke(gcnew Delegate_Invoke(this, &Form1::enable_train));
   }
 public: System::Void enable_train()
 {
     button_Train->Enabled = true;
     button_Test->Enabled = true;
     button_Abort_Train->Enabled = false;
     groupBox_Parameter->Enabled = true;
     this->Text = "Train Fuzzy-Rules: Model trained";
 }
private: System::Void button_Abort_Train_Click(System::Object^ sender, System::EventArgs^ e) 
{
    if (run_thread == true)
    {
        Test_Fuzzy->abort_train = true;
        p_Train->Join();
        Test_Fuzzy->abort_train = false;
        //
        button_Train->Enabled = true;
        button_Test->Enabled = true;
        button_Abort_Train->Enabled = false;
        groupBox_Parameter->Enabled = true;
    }
}
private: System::Void trackBar_Time_Scroll(System::Object^ sender, System::EventArgs^ e) 
{
    label_Time->Text = "Time: " + trackBar_Time->Value.ToString();
    test_Model();
}
private: System::Void trackBar_Quality_Scroll(System::Object^ sender, System::EventArgs^ e) 
{
    label_Quality->Text = "Quality: " + trackBar_Quality->Value.ToString();
    test_Model();
}
private: System::Void trackBar_Price_Scroll(System::Object^ sender, System::EventArgs^ e) 
{
    label_Price->Text = "Price: " + trackBar_Price->Value.ToString();
    test_Model();
}
private: System::Void test_Model()
{
    if (run_thread == false)
    {
        float price = trackBar_Price->Value;
        float quality = trackBar_Quality->Value / 100.0f;
        float time = trackBar_Time->Value;
        //
        float FZ_Tip;
        float Tip_Model;
        Test_Fuzzy->test_model(price, time, quality, FZ_Tip, Tip_Model);
        //
        this->Text = "Test: Fuzzy=" + FZ_Tip.ToString("n2") + " Model=" + Tip_Model.ToString("n2");
        //
        draw_Fuzzy();
    }
}
private: System::Void Form1_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) 
{
    delete Test_Fuzzy;
}
private: System::Void numericUpDown_Batch_Size_ValueChanged(System::Object^ sender, System::EventArgs^ e) 
{
    Test_Fuzzy->train_batch_size = Convert::ToInt32(numericUpDown_Batch_Size->Value);
}
private: System::Void numericUpDown_Hidden_Nodes_ValueChanged(System::Object^ sender, System::EventArgs^ e) 
{
    Test_Fuzzy->create_network(Convert::ToInt32(numericUpDown_Hidden_Nodes->Value),
                               Convert::ToInt32(numericUpDown_Layer->Value),
        get_current_Activation());
    show_network();
}
private: System::Void show_network()
{
    listBox_train_info->Items->Clear();
    for (int k = 0; k < Test_Fuzzy->str_network.size(); k++)
    {
        String^ str = gcnew String(Test_Fuzzy->str_network[k].c_str());
        listBox_train_info->Items->Add(str);
    }
}
private: System::Void numericUpDown_Learning_Rate_ValueChanged(System::Object^ sender, System::EventArgs^ e) 
{
    Test_Fuzzy->learning_rate = Convert::ToSingle(numericUpDown_Learning_Rate->Value);
}
private: System::Void numericUpDown_Layer_ValueChanged(System::Object^ sender, System::EventArgs^ e) 
{
    Test_Fuzzy->create_network(Convert::ToInt32(numericUpDown_Hidden_Nodes->Value),
        Convert::ToInt32(numericUpDown_Layer->Value), get_current_Activation());
    show_network();
}
private: System::Void button_Test_Click(System::Object^ sender, System::EventArgs^ e) 
{
    if (run_thread == false)
    {
        listBox_train_info->Items->Clear();
        D_Data->clear_test();
        Thread^ p_Test = gcnew Thread(gcnew ThreadStart(this, &Form1::test));
        p_Test->Start();
        button_Train->Enabled = false;
        button_Abort_Train->Enabled = false;
        button_Test->Enabled = false;
        groupBox_Parameter->Enabled = false;
    }
}
 private: System::Void test()
 {
     run_thread = true;
     Test_Fuzzy->test_main(Convert::ToInt32(numericUpDown_Test_Samples->Value));
     run_thread = false;
     this->BeginInvoke(gcnew Delegate_Invoke(this, &Form1::enable_train));
 }
private: System::Void numericUpDown_Log_Intervall_ValueChanged(System::Object^ sender, System::EventArgs^ e) 
{
    Test_Fuzzy->log_interval = Convert::ToInt32(numericUpDown_Log_Intervall->Value);
}
private: System::Void button_Save_Click(System::Object^ sender, System::EventArgs^ e) 
{
    if (folderBrowserDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
    {
        int N_Hidden = Convert::ToInt32(numericUpDown_Hidden_Nodes->Value);
        int N_Layer = Convert::ToInt32(numericUpDown_Layer->Value);
        //
        String^ file_name = folderBrowserDialog1->SelectedPath +
            "Model" + 
            "_H_" + N_Hidden.ToString()+ 
            "_L_" + N_Layer.ToString() + ".pt";
        //
        Test_Fuzzy->save_net(convert_String(file_name));
       
    }
    
    //Test_Fuzzy->
}
private: System::Void button_Load_Click(System::Object^ sender, System::EventArgs^ e)
{
    openFileDialog1->InitialDirectory = "c:\\";
    openFileDialog1->Filter = "pt files (*.pt)|*.pt";
    //
    if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
    {
        String^ current_filename = openFileDialog1->FileName;
        std::string filename = convert_String(current_filename);
        //
        int pos_last = current_filename->LastIndexOf("\\");
        String^ cut = current_filename->Substring(pos_last+1);
        array<String^>^ cols = cut->Split('_','.');
        //
        int N_Hidden = Convert::ToInt32(cols[2]);
        int N_Layer  = Convert::ToInt32(cols[4]);
        //
        Test_Fuzzy->create_network(N_Hidden,
            N_Layer, get_current_Activation());
        show_network();
        //
        Test_Fuzzy->load_net(filename);
    }
}
       std::string convert_String(String^ str_in)
       {
           System::IntPtr pointer = Marshal::StringToHGlobalAnsi(str_in);
           char* charPointer = reinterpret_cast<char*>(pointer.ToPointer());
           std::string Standard_String(charPointer, str_in->Length);
           Marshal::FreeHGlobal(pointer);
           //
           return Standard_String;
       }
private: System::Void comboBox_Act_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) 
{
    Test_Fuzzy->create_network(Convert::ToInt32(numericUpDown_Hidden_Nodes->Value),
        Convert::ToInt32(numericUpDown_Layer->Value), get_current_Activation());
    show_network();
}
private:  Activation_Function get_current_Activation()
       {
    Activation_Function Act;
    //
    switch (comboBox_Act->SelectedIndex)
    {
    case 0:
        Act = Activation_Relu;
        break;
    case 1:
        Act = Activation_ELU;
        break;
    case 2:
        Act = Activation_Leaky_Relu;
        break;
    case 3:
        Act = Activation_Gelu;
        break;
    case 4:
        Act = Activation_tanh;
        break;
    case 5:
        Act = Activation_Sigmoid;
        break;
    }
    return Act;
       }
}; // end of class Form1
} // end of namespace CppCLRWinFormsProject

