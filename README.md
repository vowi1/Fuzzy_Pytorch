
The project is a simple example for the usage of the pytorch C++-frontend.
In  this case some simple fuzzy rules are approximated by a neural network.
The classic tipping example is used, where the tip in a restaurent is dependent on the price, waiting time and food quality.
For visualization winforms C++/CLI is used, which can be combined with the pytorch libraries. 
Fuzzy_Pytorch.vcxproj contains the path to the Pytorch C++-Libraries:

<MY_LIBTORCH_DIR>C:\Users\vowi\Documents\Visual Studio 2022\Projects\Libtorch\</MY_LIBTORCH_DIR>

You have to download the C++-Frontend-Libraries from https://pytorch.org/ and than set the variable to the location.
Otherwise you need Visual Studio with .Net/Desktop-Development (check C++/CLI-Support). 
