package com.mci.heating.sensor.impl;

import java.util.Random;

import com.mci.heating.api.HeatingControllerService;
import org.osgi.framework.ServiceRegistration;
import org.osgi.service.component.annotations.Component;
import com.mci.heating.api.HeatingSensorService;


public class HeatingSensorServiceImpl implements HeatingSensorService{

	private static Random dice = new Random();
	
	// Returns the current temperature measured by the sensor
	// This implementation just returns a random number between 15.0 and 30.0
	@Override
	public double getSensorTemperature() {		
		return 15.0 + dice.nextDouble() * 15.0;
	}

}
