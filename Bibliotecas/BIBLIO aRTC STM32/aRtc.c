/*
 * aRtc.c
 *
 *  Created on: Oct 17, 2023
 *      Author: pablo-jean
 */


#include "aRtc.h"

void aRtc_set(RTC_HandleTypeDef *hrtc, struct tm Time){
	time_t utc;

	utc = mktime(&Time);

	__HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
	hrtc->Instance->CNTL = (utc & 0xFFFF);
	hrtc->Instance->CNTH = (utc >> 16);
	__HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
}

void aRtc_get(RTC_HandleTypeDef *hrtc, struct tm *Time){
	time_t utc;
	struct tm *local;

	utc =  (hrtc->Instance->CNTH << 16);
	utc |= hrtc->Instance->CNTL;
	// TODO: The time_t is a uint64_t

	local = localtime(&utc);

	*Time = *local;
}
