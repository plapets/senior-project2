
#ifndef GPIO_H
#define GPIO_H

/*
File:   gpio.h
Date:   2019-02-22
Author: Peter Lapets

Description:
This file contains declarations of functions used for manipulation of
general purpose I/O as exposed by the /sys/class/gpio interface.
*/

#include "RunProcessByFormat.h"


#define GPIO_LOW            (0)
#define GPIO_HIGH           (1)

#define GPIO_INPUT          ("in")
#define GPIO_OUTPUT_LOW     ("low")

#define GPIO_PATH "./scripts/gpio/"

#define GPIO_EXPORT         (GPIO_PATH "gpio-export %d:"            )
#define GPIO_UNEXPORT       (GPIO_PATH "gpio-unexport %d:"          )

#define GPIO_SET_DIRECTION  (GPIO_PATH "gpio-set-direction %d %s:"  )

#define GPIO_GET_VALUE      (GPIO_PATH "gpio-get-value %d:%d"       )
#define GPIO_SET_VALUE      (GPIO_PATH "gpio-set-value %d %d:"      )


int GpioExport      ( int gpioNumber );
int GpioUnexport    ( int gpioNumber );
int GpioSetDirection( int gpioNumber, char * direction  );
int GpioGetValue    ( int gpioNumber, int * value       );
int GpioSetValue    ( int gpioNumber, int value         );

#endif /* GPIO_H */

