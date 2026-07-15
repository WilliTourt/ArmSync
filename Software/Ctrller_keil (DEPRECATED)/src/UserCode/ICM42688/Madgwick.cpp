#include "Madgwick.h"
#include "cpp_main.h"

/*==============================================================================================*/
/*=========================================Madgwick方法==========================================*/ 
/*==============================================================================================*/

uint32_t EULER::getTick(void)
{
	// need find out work out
	
	// return HAL_GetTick();
    
    return sysTick;
}

/*==============================================================================================*/

float EULER::invSqrt(float x)
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

void EULER::Norm_Data(float *x,float *y,float *z)
{
	float recipNorm;

	recipNorm = invSqrt(*x * *x + *y * *y + *z * *z);
	*x *= recipNorm;
	*y *= recipNorm;
	*z *= recipNorm;
}

void EULER::Norm_Data_Q(float *w,float *x,float *y,float *z)
{
	float recipNorm;

	recipNorm = invSqrt(*w * *w + *x * *x + *y * *y + *z * *z);
	*w *= recipNorm;
	*x *= recipNorm;
	*y *= recipNorm;
	*z *= recipNorm;
}

float EULER::lowPassFilter(float value, float last_value, uint8_t alpha_shift) 
{
	if(alpha_shift == 0)
		return value;

    last_value = last_value + ((value - last_value) / pow(2 , alpha_shift));

    return last_value * pow(2 , alpha_shift);
}

//--------------------------------------Madgwick算法-------------------------------------------------------------
// IMU algorithm update

EULER::EULER(void)
{
	_betaDef = 0.88f;
	_last_time = getTick();

	_q0 = 1.0f;
	_q1 = 0.0f;
	_q2 = 0.0f;
	_q3 = 0.0f;
}

EULER::EULER(float betaDef) 
	: _betaDef(betaDef)
{
	_last_time = getTick();

	_q0 = 1.0f;
	_q1 = 0.0f;
	_q2 = 0.0f;
	_q3 = 0.0f;
}

/*
 * name:无磁力计Madgwick方法
 * func:无磁力计下的Madgwick方法,用于计算姿态解算
 * in:acc[3]:角速度的x,y,z轴	gyro[3]:角速度的x,y,z轴
 * out:void
 * PS:未使用该方法的yaw轴
 *
 * */
void EULER::MadgwickAHRSupdateIMU(float ax, float ay, float az, float gx, float gy, float gz)
{
	float dt = (getTick() - _last_time) / 1000.0f;
	_last_time = getTick();

	float ax_g = ax / g;
	float ay_g = ay / g;
	float az_g = az / g;
	float gx_rad = gx * PI / 180.0f * 2 * PI;	// Warning: 这里是一个很奇怪的问题
	float gy_rad = gy * PI / 180.0f * 2 * PI;	// 输入的角速度单位是°/s,然后先(* PI / 180)转换为rad/s以方便后续计算
	float gz_rad = gz * PI / 180.0f * 2 * PI;	// 但是实际发现yaw轴转动90°角度只变换到大概14°左右,恰巧约为(2 * PI)倍数关系,说明以前的单位是转/s?或者是四元数计算公式有问题

	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

	// Rate of change of quaternion from gy_radroscope
	qDot1 = 0.5f * (-_q1 * gx_rad - _q2 * gy_rad - _q3 * gz_rad);
	qDot2 = 0.5f * (_q0 * gx_rad + _q2 * gz_rad - _q3 * gy_rad);
	qDot3 = 0.5f * (_q0 * gy_rad - _q1 * gz_rad + _q3 * gx_rad);
	qDot4 = 0.5f * (_q0 * gz_rad + _q1 * gy_rad - _q2 * gx_rad);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax_g == 0.0f) && (ay_g == 0.0f) && (az_g == 0.0f))) {

		// Normalise accelerometer measurement
		Norm_Data(&ax_g,&ay_g,&az_g);

		// Auxiliary variables to avoid repeated arithmetic
		_2q0 = 2.0f * _q0;
		_2q1 = 2.0f * _q1;
		_2q2 = 2.0f * _q2;
		_2q3 = 2.0f * _q3;
		_4q0 = 4.0f * _q0;
		_4q1 = 4.0f * _q1;
		_4q2 = 4.0f * _q2;
		_8q1 = 8.0f * _q1;
		_8q2 = 8.0f * _q2;
		q0q0 = _q0 * _q0;
		q1q1 = _q1 * _q1;
		q2q2 = _q2 * _q2;
		q3q3 = _q3 * _q3;

		// Gradient decent algorithm corrective step
        s0 = _4q0 * q2q2 + _2q2 * ax_g + _4q0 * q1q1 - _2q1 * ay_g;
        s1 = _4q1 * q3q3 - _2q3 * ax_g + 4.0f * q0q0 * _q1 - _2q0 * ay_g - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az_g;
        s2 = 4.0f * q0q0 * _q2 + _2q0 * ax_g + _4q2 * q3q3 - _2q3 * ay_g - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az_g;
        s3 = 4.0f * q1q1 * _q3 - _2q1 * ax_g + 4.0f * q2q2 * _q3 - _2q2 * ay_g;
		Norm_Data_Q(&s0,&s1,&s2,&s3);

		// Apply feedback step
		qDot1 -= _betaDef * s0;
		qDot2 -= _betaDef * s1;
		qDot3 -= _betaDef * s2;
		qDot4 -= _betaDef * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	_q0 += qDot1 * dt;
	_q1 += qDot2 * dt;
	_q2 += qDot3 * dt;
	_q3 += qDot4 * dt;

	// Normalise quaternion
	Norm_Data_Q(&_q0,&_q1,&_q2,&_q3);
}

/*
 * name:Madgwick方法
 * func:有磁力计下的Madgwick方法,用于计算姿态解算
 * in:acc[3]:角速度的x,y,z轴	gyro[3]:角速度的x,y,z轴	mag[3]:磁力计的x,y,z轴
 * out:void
 * PS:未使用该方法的yaw轴
 *
 * */
void EULER::MadgwickAHRSupdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
	// Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
	if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f)) {
		MadgwickAHRSupdateIMU(ax, ay, az, gx, gy, gz);
		return;
	}

	float dt = (getTick() - _last_time) / 1000.0f;
	_last_time = getTick();

	float ax_g = ax / g;
	float ay_g = ay / g;
	float az_g = az / g;
	float gx_rad = gx * PI / 180.0f * 2 * PI;	// Warning: 这里是一个很奇怪的问题
	float gy_rad = gy * PI / 180.0f * 2 * PI;	// 输入的角速度单位是°/s,然后先(* PI / 180)转换为rad/s以方便后续计算
	float gz_rad = gz * PI / 180.0f * 2 * PI;	// 但是实际发现yaw轴转动90°角度只变换到大概14°左右,恰巧约为(2 * PI)倍数关系,说明以前的单位是转/s?或者是四元数计算公式有问题

    float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
    float hx, hy;
	float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

	// Rate of change of quaternion from gy_radroscope
	qDot1 = 0.5f * (-_q1 * gx_rad - _q2 * gy_rad - _q3 * gz_rad);
	qDot2 = 0.5f * (_q0 * gx_rad + _q2 * gz_rad - _q3 * gy_rad);
	qDot3 = 0.5f * (_q0 * gy_rad - _q1 * gz_rad + _q3 * gx_rad);
	qDot4 = 0.5f * (_q0 * gz_rad + _q1 * gy_rad - _q2 * gx_rad);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax_g == 0.0f) && (ay_g == 0.0f) && (az_g == 0.0f))) {

		// Normalise accelerometer measurement
		Norm_Data(&ax_g,&ay_g,&az_g);

		// Normalise magnetometer measurement
		Norm_Data(&mx,&my,&mz);

		// Auxiliary variables to avoid repeated arithmetic
		_2q0mx = 2.0f * _q0 * mx;
		_2q0my = 2.0f * _q0 * my;
		_2q0mz = 2.0f * _q0 * mz;
		_2q1mx = 2.0f * _q1 * mx;
		_2q0 = 2.0f * _q0;
		_2q1 = 2.0f * _q1;
		_2q2 = 2.0f * _q2;
		_2q3 = 2.0f * _q3;
		_2q0q2 = 2.0f * _q0 * _q2;
		_2q2q3 = 2.0f * _q2 * _q3;
		q0q0 = _q0 * _q0;
		q0q1 = _q0 * _q1;
		q0q2 = _q0 * _q2;
		q0q3 = _q0 * _q3;
		q1q1 = _q1 * _q1;
		q1q2 = _q1 * _q2;
		q1q3 = _q1 * _q3;
		q2q2 = _q2 * _q2;
		q2q3 = _q2 * _q3;
		q3q3 = _q3 * _q3;

		// Reference direction of Earth's magnetic field
		hx = mx * q0q0 - _2q0my * _q3 + _2q0mz * _q2 + mx * q1q1 + _2q1 * my * _q2 + _2q1 * mz * _q3 - mx * q2q2 - mx * q3q3;
		hy = _2q0mx * _q3 + my * q0q0 - _2q0mz * _q1 + _2q1mx * _q2 - my * q1q1 + my * q2q2 + _2q2 * mz * _q3 - my * q3q3;
		_2bx = sqrt(hx * hx + hy * hy);
		_2bz = -_2q0mx * _q2 + _2q0my * _q1 + mz * q0q0 + _2q1mx * _q3 - mz * q1q1 + _2q2 * my * _q3 - mz * q2q2 + mz * q3q3;
		_4bx = 2.0f * _2bx;
		_4bz = 2.0f * _2bz;

		// Gradient decent algorithm corrective step
		s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax_g) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay_g) - _2bz * _q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * _q3 + _2bz * _q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * _q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax_g) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay_g) - 4.0f * _q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az_g) + _2bz * _q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * _q2 + _2bz * _q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * _q3 - _4bz * _q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax_g) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay_g) - 4.0f * _q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az_g) + (-_4bx * _q2 - _2bz * _q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * _q1 + _2bz * _q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * _q0 - _4bz * _q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax_g) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay_g) + (-_4bx * _q3 + _2bz * _q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * _q0 + _2bz * _q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * _q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		Norm_Data_Q(&s0,&s1,&s2,&s3);

		// Apply feedback step
		qDot1 -= _betaDef * s0;
		qDot2 -= _betaDef * s1;
		qDot3 -= _betaDef * s2;
		qDot4 -= _betaDef * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	_q0 += qDot1 * dt;
	_q1 += qDot2 * dt;
	_q2 += qDot3 * dt;
	_q3 += qDot4 * dt;

	// Normalise quaternion
	Norm_Data_Q(&_q0,&_q1,&_q2,&_q3);
}
