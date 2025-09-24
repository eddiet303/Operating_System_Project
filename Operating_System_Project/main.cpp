#include<iostream>
#include<string>
#include "auth.h"

using namespace std;

int main() {

	string username, password;
	cout << "OS is booting up..." << endl;

	int attempts = 0;
	const int maxAttempts = 3;

	while (attempts < maxAttempts){
	cout << "Enter username: ";
	cin >> username;
	cout << endl;
	cout << "Enter password: ";
	cin >> password;
	cout << endl;

	if (authenticateUser(username, password)) {
		break;
	}
		attempts++;
}
	return 0;
}
