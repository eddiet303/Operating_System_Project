#include<iostream>
#include<string>
//New
#include "auth.h"
//#include "Process.h"
//#include <thread>   // for sleep_for
//#include <chrono>   // for seconds

//simulate time passing and print each step as the process changes states
// To simulate we need a loop in main.cpp to “drive” process state
//#include "Process.h"
//
using namespace std;


//New loop to simulte the a running process



/////////////////////////////////////////
int main() {

	//Variables for user input
	string username, password;
	cout << "OS is booting up..." << endl;

	//Track the numer of login attempts
	int attempts = 0;
	const int maxAttempts = 3;

	//loop to chack the values of string or max attemts Reached
	while (attempts < maxAttempts) {
		/*here the user input is store one time compare the user input if the input
		is not the same as the authentication credential then the user overwrite the
		value of the username again until the credential match
				*/
		cout << "Enter username: ";
		cin >> username;
		cout << endl;
		cout << "Enter password: ";
		cin >> password;
		cout << endl;

		//call the authentication function
		if (authenticateUser(username, password)) {
			break;
		}
		attempts++;
	}
	return 0;
}
