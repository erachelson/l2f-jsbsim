#include "Thermals.hpp"
#include <cstdlib>
#include <algorithm>

double dummy1=(double) rand() / (RAND_MAX);

 
double integralWzAllen(double h);

double simpsons( double (*f)(double x), double a, double b, int n); 

Thermals::Thermals():tBirth(0.0),xc0(0.0),yc0(0.0)
{
	//Thermal initialized with all default values
	zi = 1400;
	w_star = 2.64;
	lifeIndex=0.0;
	lifeTime= 15;
	restTime= 1;
	isAlive=true;
	isEffective=false;
	xi = 0.1 + (0.5 - 0.25) *(double) rand() / (RAND_MAX);
	driftx=0.0;
	drifty=0.0;
}

Thermals::Thermals(double xc, double yc, double z, double w, double tb, double tRest, double tLife):tBirth(tb),xc0(xc),yc0(yc)
{
	zi = z;       //mixing-layer thickness
	w_star =w;
	lifeIndex=0.0;
	lifeTime= tLife;
	restTime= tRest;
	isAlive=false;
	isEffective=false;
	xi = 0.1 + (0.5 - 0.25) *(double) rand() / (RAND_MAX);
	driftx=0.0;
	drifty=0.0;
}

Thermals::~Thermals()
{
}

void Thermals::updateThermal(double tSim)
//This function updates the thermal based on its life cycle and drift velocity.
//The thermal is considered dead either if it has not been born or if its lifetime is over
{
	if(tSim>=tBirth)
	{
		isAlive=true;
		lifeIndex=tSim-tBirth;
		
		if(lifeIndex>(restTime+lifeTime))
		{
			isAlive=false;
			return;
		}
		if(lifeIndex>restTime)
		{
			isEffective=true;
		}
		else
		{
			isEffective=false;
		}
		
		//updating new thermal center based on drift velocity and time elapsed since tBirth
		xc = xc0 + driftx*(tSim-tBirth);
		yc = yc0 + drifty*(tSim-tBirth);
	}
	else
	{
		isAlive=false;
		return;
	}		
}

double Thermals::timeCoeff()
/*
* This function returns the value of the thermal life cycle coefficient.
* The function has no parameters because all the required parameters are taken from the thermal object itself.
*/
{	
	if(!isAlive)
		return(0.0);

	double T,tau;
	
	T = (1+xi)/lifeTime;
	tau = lifeIndex-(restTime+lifeTime/2.0);

	if (fabs(tau) <= ((1-xi) / (2.0*T)))
		return(1);
	else if (( (1-xi)/(2.0*T) < fabs(tau) ) && ( fabs(tau) <= (1+xi)/(2.0*T) ))
		 return(0.5 * (1+cos( (PI*T)/xi * (fabs(tau) - (1-xi)/(2.0*T)))));
	else
		return(0);

}

void Thermals::calcThWind(int model,double* w,double* position,double t, double ambWindX, double ambWindY, double envSink)
{
	
	double x,y,z;
	x=position[0];
	y=position[1];
	z=position[2];
	
	updateThermal(t);
	if(timeCoeff()==0.0)
	{
		return;//Thermal is not alive
	}
	double r;
	double w_total;
	double w_e=envSink;
	
	//Calculate distance based on Ambient Wind
	r=distToUpdraftCenter(x,y,z,ambWindX, ambWindY);
	switch(model)
	{
	case 1:
		//updraft Calculation based on Lenschow with Gaussian distribution
		w[0]=0.0;
		w[1]=0.0;
		w[2]=Lenschow(r,z,1)*timeCoeff();
		return;
	case 2:
		//updraft Calculation based on Lenschow with Geodon model
		w[0]=0.0;
		w[1]=0.0;
		w[2]=Lenschow(r,z)*timeCoeff();
		return;
	case 3:
		//updraft calculation based on Allen model
		w[0]=0.0;
		w[1]=0.0;
		w[2]=Allen(r,z,w_e)*timeCoeff();	
		return;
	
	case 4:
		//Calculation using Childress model
		w[0]=0.0;
		w[1]=0.0;
		w[2]=Childress(r, z)*timeCoeff();
		return;
	
	case 5:
		//Calculating using Lawarance model
		Lawrance(w,x,y,z,t,ambWindX, ambWindY);
		w[0]*=timeCoeff();
		w[1]*=timeCoeff();
		w[2]*=timeCoeff();
		return;
	}
}

double Thermals::distToUpdraftCenter(double x, double y, double z, double Wx,double Wy)
{
	double r,xcz,ycz;
	
	xcz=xc + ((Wx!=0.0)? simpsons(integralWzAllen,100.0,z,1000)*Wx : 0.0);
	ycz=yc + ((Wy!=0.0)? simpsons(integralWzAllen,100.0,z,1000)*Wy : 0.0);
	r=sqrt((xcz-x)*(xcz-x) + (ycz-y)*(ycz-y));
	return(r);	
}
double integralWzAllen(double h)
{	
	double output;
	if(h>1100)
		h=1100;
	output = 1/(2.64 * pow((h/1400.0),1.0/3.0) * (1.0 - 1.1*h/1400.0));
	return(output);
}

void Thermals::Lawrance(double* w,double x, double y, double z, double t,double Wx,double Wy)
{
	double x0=0.0,y0=0.0,z0=0.0,xt,yt,zt;
	w[0]=0.0;
	w[1]=0.0;
	w[2]=0.0;
	
	double rT,r1,r1_r2=0.36, k=3.0;
	rT = 0.102 * pow((z / zi),1.0/3.0) * (1.0 - 0.25*z/zi) * zi;
	rT = std::max(10.0, rT);

	r1 = r1_r2*rT;
	double w_,w_core ; 
	
	//Calculating w_ and W_peak using
	w_ = w_star * pow((z / zi),1.0/3.0) * (1.0 - 1.1*z/zi);
	w_core = 3.0 * w_ * (rT*rT*rT - r1*rT*rT) / (rT*rT*rT - r1*r1*r1);
	
	//The value of Z0 used below has to be modified if the movemenet of the bubble along the vertical direction has to be modeled.
	// a constant Z0 means the thermal bubble doesn't move along the vertical direction and stays at the same height 
        z0 = 800.0; 
	if(z0<k*rT)
	//The bubble has not detached from the ground yet
	{
		x0=xc;
		y0=yc;		
	}
	else
	//The bubble is completely formed ans it can detach itself from the ground and move along with the wind
	{
		x0=xc + simpsons(integralWzAllen,100.0,z,1000)*Wx;
		y0=yc + simpsons(integralWzAllen,100.0,z,1000)*Wy;
	}
	
	zt=z-z0;
	xt=x-x0;
	yt=y-y0;
	
	double dH;
	dH=sqrt(xt*xt + yt*yt);

	
	//Calculation of Wz
	if(dH==0)
	{
		w[2]= w_core * 0.5*(cos(PI*zt/(k*rT))+1.0);		
	}
	else if(dH<=2*rT)
	{
		w[2]=0.5*(w_core*rT)/(PI*dH) * sin(PI*dH/rT) * (cos(PI*zt/(k*rT))+1.0);
	}
	else
	{
		w[2]=0.0;
	}
	
	if(fabs(zt)>(k*rT))
	{
		w[2]=0.0;
	}
	//calculation of Wx Wy
	if(dH!=0.0||dH<rT)
	{
		w[0]=-(w[2]*zt*xt)/(dH*(dH-rT)*k*k);
		w[1]=-(w[2]*zt*yt)/(dH*(dH-rT)*k*k);
	}
	else if(dH==rT)
	{
		w[0]=-w_core/(2.0*k*rT) * (1.0 + cos(PI*z/(k*rT)));
		w[1]=-w_core/(2.0*k*rT) * (1.0 + cos(PI*z/(k*rT)));
	}
	else
	{
		w[0]=0;
		w[1]=0;
	}
}

double Thermals::Lenschow(double r, double z, int choice)
{
	if(z>zi)
	//	The flight level is higher than the CBL so no effect of thermal updraft. 
		return(0.0);
		
	double w_total,w_,d;
	double w_peak;
	//The diameter of the thermal is given by
	d = 0.16 * pow((z / zi),1.0/3.0) * (1 - 0.25*z/zi) * zi;

	//normalized updraft velocity
	w_= w_star*pow((z / zi),1.0/3.0) * (1 - 1.1*z/zi);
	//variance of the updraft velocity
	double var=1.8*pow((z / zi),2.0/3.0) * (1-0.8*z/zi)*(1 - 0.8*z/zi);
	
	//w_peak assuming a gaussian distribution is 
	w_peak=w_;
	if (choice==1)	
		w_total=w_peak*exp(-(4*r*r/(d*d)));
	else 
		w_total=w_peak*exp(-(4*r*r/(d*d)))*(1-(4*r*r/(d*d)));	

	return(w_total);
}

double Thermals::Allen(double r, double z, double w_e)
{
	/*
	 * Calculation of Updraft
	 * Reference:
	 *  Allen, M. J.,  Updraft Model for development of Autonomous Soaring Uninhabited Air Vehicles,
	 *  Techrep., NASA Dryden Flight Research Center, 2006.
	 */
	int NB_THERMALS=1;
	
	if(z>zi)
	//	The flight level is higher than the CBL so no effect of thermal updraft. 
		return(0.0);
	
	double w_total;
	double w_peak,wd, w_ ,wL,s_wd;
	double r1,r2;
	
	//Constants used for calculating updraft
	double r1_r2,k1,k2,k3,k4;
	
	r1_r2 = 0.36;    //r1 / r2
	k1 = 1.4866;     //ki values valid for r1 / r2 = 0.36
	k2 = 4.8354;
	k3 = -0.0320;
	k4 = 0.0001;
	
	/*
	r1_r2 = 0.47;
	k1 = 1.2042;     //ki values valid for r1 / r2 = 0.47
	k2 = 7.7904;
	k3 = 0.0848;
	k4 = 0.0001;
	*/
	
	r2 = 0.102 * pow((z / zi),1.0/3.0) * (1.0 - 0.25*z/zi) * zi;
	r2 = std::max(10.0, r2);
	//cout<<r2<<endl;
	r1 = r1_r2*r2;
	//Calculating w_ and W_peak using
	w_ = w_star * pow((z / zi),1.0/3.0) * (1.0 - 1.1*z/zi);
	w_peak = 3.0 * w_ * (r2*r2*r2 - r1*r2*r2) / (r2*r2*r2 - r1*r1*r1);
	//Checking for downdraft conditions
	if ((r2 < r) && (r < (2.0 * r2)))
		wL = PI/6.0 * sin(PI * r / r2);
	else
		wL = 0;

	if ((0.5 < (z / zi))&& ((z / zi) < 0.9))
	{
		s_wd = 2.5 * (z/zi - 0.5);
		wd = s_wd * wL;
		if (wd >0)
			wd = 0;
	}
	else
	{ 
		wd = 0;
		s_wd = 0;
	}

	//Total updraft
	w_total = w_peak * ( 1/(1+pow((fabs(k1*r/r2 + k3)),k2)) + k4 * r / r2 + wd );
	
	//Stretch updraft to blend with sink at edge
	if ((r > r1) && (w_peak != 0))
		w_total = w_total * (1 - w_e/w_peak) + w_e;
	
	if(r>2.0*r2)
		w_total=0.0;
		
	return(w_total);
			
}

double Thermals::Childress(double r, double z)
{
	/*
	Reference:
		An Empirical Model of  ermal Updra s Using Data Obtained From a Manned Glider
		Christopher E. Childress
	*/
	double w_total, w_peak,wd,w_dec, w_;
	double d_T;
	double r1,r2;
	double z_star;
	z_star = z/zi;	

	//Calculation of radius of the thermal 			
	d_T = zi*(0.4 * pow(z_star,1.0/3.0) * (1 - 0.5*z_star)) + (z*z_star*z_star - 0.6*z*z_star)/PI;
	r2 = 0.5*d_T;
	
	//Core downdraft radius
	r1 = 0.5*(0.17*d_T + 0.5*(z_star - 0.6)*d_T);

	// Calculating w_ and w_peak based on Allen
	w_ = w_star * pow((z / zi),1.0/3.0) * (1 - 1.1*z/zi);
	w_peak = 3 * w_ * (r2*r2*r2 - r1*r2*r2) / (r2*r2*r2 - r1*r1*r1);
	
	// Calculation of downdraft terms
	w_dec = (-zi/(1.275*w_star*w_star))*(12.192/z);
	wd = w_dec*(z_star + 0.45) - 0.5*w_peak;
	
	if(z_star>1.0)
	//The flight level is higher than the CBL so no effect of thermal updraft.
	{
		return(0.0);
	}
	
	//Calculation of Updraft based on equations 14,15,16 from Childress
	
	if(z_star<0.5 && r<=r2)
	{
		w_total = w_peak*cos((r/r2)*PI*0.5);
	}
	else if(z_star<0.9)
	{
		if(r<r1)
		{
			w_total = wd*cos((r/r1)*PI*0.5);
		}
		else if(r1<=r && r<=(r1+r2))
		{
			w_total = w_peak*sin((r-r1)/r2 * 1.212 *PI);
			
			if(w_total<0.0)
				w_total=0.0;
		} 
		else
		{
			w_total = 0.0;
		}
	}
	else
	{
		if(r<r1)
		{
			w_total = 0.5*wd*cos((r/r1)*PI*0.5);
		}
		else if(r1<=r && r<=(r1+r2))
		{
			w_total = (1-z_star)*w_peak*sin((r-r1)/r2 * 1.212 *PI);
			
			if(w_total<0.0)
				w_total=0.0;
		}
		else
		{
			w_total=0.0;
		}				
	}
	return(w_total);
}

double simpsons( double (*f)(double x), double a, double b, int n) 
{
        double h = (b - a) / n;
        double x;
        double r;
        char m = 0;
        double s = 0.0;

        for (x = a; x <= b; x+=h) 
        {
                r = f(x);
                if (x == a || x == b) 
                {
                        s += r;
                } 
                else 
                {
                        m = !m;
                        s += r * (m+1) * 2.0;
                }
        }
        return s * (h/3.0);
}

