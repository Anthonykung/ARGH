#include <ctime>
#include <iostream>

using namespace std;

int main()
{
	time_t initialtime = time(nullptr);
while(1){
	time_t result = time(nullptr);
	result = result - initialtime;
	cout << asctime(localtime(&result))
		<< result << " seconds since the Epoch\n";
}
}
