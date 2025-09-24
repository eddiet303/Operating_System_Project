#include<iostream>
#include<string>
#include "auth.h"

using namespace std;

int main() {

	//Variables for user input
	string username, password;
	cout << "OS is booting up..." << endl;
	
	//Track the numer of login attempts
	int attempts = 0;
	const int maxAttempts = 3;

	//loop to chack the values of string or max attemts Reached
	while (attempts < maxAttempts){
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
