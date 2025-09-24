/*
auth. h declares the function authenticateUser
*/

#pragma once
#ifndef AUTH_H
#define AUTH_H
#include<string>

bool authenticateUser(const std::string validUsername, const std::string validPassword);

#endif
