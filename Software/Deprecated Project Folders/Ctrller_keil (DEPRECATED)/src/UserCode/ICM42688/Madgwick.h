#ifndef __MADGWICK_H__
#define __MADGWICK_H__

#include <math.h>

#include <stdint.h>

#define PI 3.14159265
#define g  9.80655

class EULER
{
    public:
        EULER(void);
        EULER(float betaDef);

        void MadgwickAHRSupdateIMU(float ax, float ay, float az, float gx, float gy, float gz);
        void MadgwickAHRSupdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);

        inline float get_q0() { return _q0; }
        inline float get_q1() { return _q1; }
        inline float get_q2() { return _q2; }
        inline float get_q3() { return _q3; }

        inline float get_pitch() { _pitch = asinf(-2.0f * (_q1*_q3 - _q0*_q2)) * (180.0f / PI); return _pitch; }
        inline float get_roll()  { _roll = atan2f(_q0*_q1 + _q2*_q3, 0.5f - _q1*_q1 - _q2*_q2) * (180.0f / PI); return _roll; }
        inline float get_yaw()   { _yaw = atan2f(2.0f*(_q1*_q2 + _q0*_q3), _q0*_q0 + _q1*_q1 - _q2*_q2 - _q3*_q3) * (180.0f / PI); return _yaw; }

    private:
        uint32_t getTick(void);
        float invSqrt(float x);
        void Norm_Data(float *x,float *y,float *z);
        void Norm_Data_Q(float *w,float *x,float *y,float *z);
        float lowPassFilter(float value, float last_value, uint8_t alpha_shift);

        float _betaDef;
        float _q0, _q1, _q2, _q3;
        float _pitch;
        float _roll;
        float _yaw;

        uint32_t _last_time;
};

#endif
