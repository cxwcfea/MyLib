/*
 * main.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: xcheng
 */

#include "Precompiled.h"

using namespace std;

int main(int argc, char **argv) {
	auto a = 9;
	TRACE("Start %d\n", a);
	auto b = make_unique<int>(8);
	TRACE("%d\n", *b);
}



