#ifndef __CONFIG_ENV_HPP__
#define __CONFIG_ENV_HPP__
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <iostream>
#include <bits/c++config.h>
#include <bits/stringfwd.h> 	// For string forward declarations.
#include <bits/postypes.h>
#include <string>

int createDir(const char *sPathName) ;
int writeFile(const char *file_name, const char *contents, bool append);
int setCameraEnv();

#endif // !__CONFIG_ENV_HPP__