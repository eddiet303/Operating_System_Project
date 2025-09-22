#include "auth.h"
#include<iostream>
#include<string>

using namespace std;

bool authenticateUser(string validUsername, string validPassword){

	const string validUser = "Administrator";
	const string validPass = "Hello123#";

	if (validUsername == validUser && validPassword == validPass) {
		cout << "Access Granted. Welcome " << validUsername << endl;
	}
	else {
		cout << "Invalid Username or Password. Try again." << endl << endl;
	}
	return (validUsername == validUser && validPassword == validPass);
}
