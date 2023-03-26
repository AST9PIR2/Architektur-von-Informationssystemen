package com.mci.heating.api;

public interface HeatingControllerService {
	
	void setTargetTemperature(double temp);
	
	double getTargetTemperature();
	
}
