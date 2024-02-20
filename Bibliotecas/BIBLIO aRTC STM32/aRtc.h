/*
 * aRtc.h
 *
 *  Created on: Oct 17, 2023
 *      Author: pablo-jean
 */

#ifndef ARTC_H_
#define ARTC_H_

#include <time.h>

#include "stm32f1xx.h"

void aRtc_set(RTC_HandleTypeDef *hrtc, struct tm Time);

void aRtc_get(RTC_HandleTypeDef *hrtc, struct tm *Time);

#endif /* ARTC_H_ */
