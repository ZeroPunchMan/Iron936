#include "pid.h"

void PIDController_Init(PIDController *pid)
{

	/* Clear controller variables */
	pid->integrator = 0.0f;
	pid->prevError = 0.0f;

	pid->prevMeasurement = 0.0f;

	pid->out = 0.0f;
}

float PIDController_Update(PIDController *pid, float setpoint, float measurement)
{

	/*
	 * Error signal
	 */
	float error = setpoint - measurement;

	/*
	 * Proportional
	 */
	float proportional = pid->Kp * error; // 比例部分

	/*
	 * Integral
	 */
	pid->integrator = pid->integrator + 0.5f * pid->Ki * pid->T * (error + pid->prevError); // 积分部分,取当前误差和上次误差的均值

	/* Anti-wind-up via integrator clamping */
	if (pid->integrator > pid->limMaxInt) // 积分项范围限制
	{
		pid->integrator = pid->limMaxInt;
	}
	else if (pid->integrator < pid->limMinInt)
	{
		pid->integrator = pid->limMinInt;
	}

	/*
	 * Derivative (band-limited differentiator)
	 */
	/* Note: derivative on measurement, therefore minus sign in front of equation! */
	float a = pid->Kd * (measurement - pid->prevMeasurement);
	float b = pid->T;
	float differentiator = a / b; // 微分项? 

	/*
	 * Compute output and apply limits
	 */
	// 输出计算,仅偏移量?
	pid->out = proportional + pid->integrator + differentiator;

	// 输出范围限制
	if (pid->out > pid->limMax)
	{
		pid->out = pid->limMax;
	}
	else if (pid->out < pid->limMin)
	{
		pid->out = pid->limMin;
	}

	/* Store error and measurement for later use */
	pid->prevError = error;
	pid->prevMeasurement = measurement;

	/* Return controller output */
	return pid->out;
}
