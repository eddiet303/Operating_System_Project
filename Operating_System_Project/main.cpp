#include<iostream>
#include<string>
#include "auth.h"

using namespace std;

int main() {

	string username, password;
	cout << "OS is booting up..." << endl;

	while (true){
	cout << "Enter username: ";
	cin >> username;
	cout << endl;
	cout << "Enter password: ";
	cin >> password;
	cout << endl;

	if (authenticateUser(username, password)) {
		break;
	}
}
	return 0;
}