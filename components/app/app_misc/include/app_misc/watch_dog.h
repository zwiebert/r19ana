/*
 * watch_dog.h
 *
 *  Created on: 04.06.2019
 *      Author: bertw
 */

#ifndef MAIN_WATCH_DOG_H_
#define MAIN_WATCH_DOG_H_

#include <stdbool.h>

bool watchDog_checkCommandLine(const char *cmdLine);
void watchDog_loop();

#endif /* MAIN_WATCH_DOG_H_ */
