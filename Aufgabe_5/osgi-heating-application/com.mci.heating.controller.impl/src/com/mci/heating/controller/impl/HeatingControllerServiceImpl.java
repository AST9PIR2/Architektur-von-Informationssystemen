package com.mci.heating.controller.impl;

import com.mci.heating.api.HeatingControllerService;
import com.mci.heating.api.HeatingDeviceService;
import com.mci.heating.api.HeatingSensorService;

import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.component.annotations.*;


import java.util.Random;


@Component(
		immediate = true // Start the bundle as soon as all dependencies are satisfied
)
public class HeatingControllerServiceImpl implements HeatingControllerService, Runnable {

	private double targetTemperature = 0.0;

	private Thread controllerThread = null;

	private HeatingSensorService sensorService = null;

	private HeatingDeviceService deviceService = null;

	private boolean shutdownFlag = false;

	private Random random = new Random();


	// Gets executed when the bundle is started
	@Activate
	public void activate(BundleContext bundleContext) {
		System.out.println("Starting HeatingControllerServiceImpl");


		try {

			// Get Service Reference for HeatingSensorService
			ServiceReference<?> sensorServiceRef = bundleContext.getServiceReference(HeatingSensorService.class);
			System.out.println("sensorServiceRef: " + sensorServiceRef);
			System.out.println("HeatingSensorService.class: " + bundleContext.getServiceReference(HeatingSensorService.class));
			if (sensorServiceRef == null) {
				throw new Exception("Could not get service reference for service HeatingSensorService");
			}

			// Get actual implementation of HeatingSensorService
			sensorService = (HeatingSensorService) bundleContext.getService(sensorServiceRef);
			if (sensorService == null) {
				throw new Exception("Could not get service HeatingSensorService");
			}

			// Get Service Reference for HeatingDeviceService
			ServiceReference<?> deviceServiceRef = bundleContext.getServiceReference(HeatingDeviceService.class);
			if (deviceServiceRef == null) {
				throw new Exception("Could not get service reference for service HeatingDeviceService");
			}

			// Get actual implementation of HeatingDeviceService
			deviceService = (HeatingDeviceService) bundleContext.getService(deviceServiceRef);
			if (deviceService == null) {
				throw new Exception("Could not get service HeatingDeviceService");
			}

			controllerThread = new Thread(this);
			controllerThread.start();

		} catch(Exception e) {
			System.err.println("Could not start controller thread: " + e);
			e.printStackTrace();
		}
	}

	// Gets executed when the bundle is stopped
	@Deactivate
	public void deactivate(BundleContext bundleContext) {
		System.out.println("Stopping HeatingControllerServiceImpl");

		if (controllerThread != null) {
			shutdownFlag = true;
			try {
				controllerThread.join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			controllerThread = null;
		}
	}

	@Override
	public void setTargetTemperature(double temp) {
		targetTemperature = temp;
	}

	@Override
	public double getTargetTemperature() {
		return targetTemperature;
	}

	@Override
	public void run() {
		while (!shutdownFlag) {
			// Ist-Temperatur holen
			double currentTemperature = sensorService.getSensorTemperature();
			System.out.println("Current temperature is " + currentTemperature);

			// Hier Regelautomatik implementieren
			int level = random.nextInt(255);
			System.out.println("Setting heating level to " + level);

			// Heizung einstellen
			deviceService.setHeatingLevel(level);
			System.out.println("Current heating level is " + deviceService.getHeatingLevel());

			try {
				Thread.sleep(3000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}

}
