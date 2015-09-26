#include "scalar.h"
#include "system.h"

SVGENDQ svgen_dq = SVGENDQ_DEFAULTS;


void ScalarControl(void)
{
	_iq Ds, Udc, angle;
	_iq sin, cos;

	Udc = _IQ(20.0); //system.Udc;
	Ds = _IQdiv(_IQmpy(system.Uref, _IQ(K_MOD_SPACEPWM)), Udc);
	if (Ds > _IQ(0.98))
		Ds = _IQ(0.98);

	system.Uout = _IQmpy( _IQmpy(Udc, Ds), _IQ(1/K_MOD_SPACEPWM));

	angle = pwm.angle;
	angle += pwm.angle_step;

	if (angle >= _IQ(1.0))
	{
		angle -= _IQ(1.0);
		pwm.period_out = 1;
	}
	pwm.angle = angle;

	sin = _IQsinPU(angle);
	cos = _IQcosPU(angle);

    svgen_dq.Ualpha = _IQmpy(Ds, cos);
    svgen_dq.Ubeta  = _IQmpy(Ds, sin);
    svgen_dq.calc(&svgen_dq);


#if 0
    _iq             Ds, Udc, Angle;
    _iq             sinT, cosT;

    Udc = ctom.measData.UdcFast;

    Ds = _IQdiv(_IQmpy(mtoc.mFControl.Uref, _IQ(K_MOD_SPACEPWM)), Udc);
    if(Ds > _IQ(0.98))  Ds = _IQ(0.98);

    ctom.cFControl.Uout = _IQmpy( _IQmpy(Udc, Ds), _IQ(1/K_MOD_SPACEPWM));

    Angle  = ctom.cFControl.Angle;
    Angle += mtoc.mFControl.StepAngle;

    if(Angle > _IQ( 1.0)){
        Angle -= _IQ(1.0);
        ctom.measData.periodOut = 1;
    }
    else if(Angle < _IQ(-1.0)){
        Angle += _IQ(1.0);
        ctom.measData.periodOut = 1;
    }
    ctom.cFControl.Angle = Angle;

    sinT = _IQsinPU(Angle);
    cosT = _IQcosPU(Angle);

    svgen_dq.Ualpha = _IQmpy(Ds, cosT);
    svgen_dq.Ubeta  = _IQmpy(Ds, sinT);
    svgen_dq.calc(&svgen_dq);

/*
    pwm1.MfuncC1 = (s16)_IQtoIQ15(svgen_dq.Tc); // MfuncC1 is in Q15
    pwm1.MfuncC2 = (s16)_IQtoIQ15(svgen_dq.Ta); // MfuncC2 is in Q15
    pwm1.MfuncC3 = (s16)_IQtoIQ15(svgen_dq.Tb); // MfuncC3 is in Q15
    pwm1.triol_update(&pwm1);

    v->Tc = (s16)_IQtoIQ15(svgen_dq.Tc);
    v->Ta = (s16)_IQtoIQ15(svgen_dq.Ta);
    v->Tb = (s16)_IQtoIQ15(svgen_dq.Tb);
*/
#endif
}

void rate_generator()
{
	static _iq time = 0;
	_iq f;

	if (system.state == running_state)
	{
		if (time < system.Tacc)
		{
			f = _IQmpy(_IQdiv(system.Fref, system.Tacc), time);
			system.Fout = f;
			time += pwm.pwm_period;

			uf_characteristics();

			pwm.angle_step = _IQmpy(system.Fout, pwm.pwm_period);
		}
		else
		{
			system.Fout = system.uf.F[1];
			system.Uref = system.uf.U[1];
		}
	}
	else if (system.state == ready_state)
	{
		time = 0;
		system.Fout = system.uf.F[0];
		system.Uref = system.uf.U[0];
	}
}



void uf_characteristics()
{
	system.Uref = _IQmpy(_IQdiv(system.uf.U[1] - system.uf.U[0], system.uf.F[1] - system.uf.F[0]), system.Fout);
}
