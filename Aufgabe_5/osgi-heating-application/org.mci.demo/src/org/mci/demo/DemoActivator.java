package org.mci.demo;

import com.mci.heating.api.HeatingSensorService;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import org.osgi.service.component.annotations.Component;


public class DemoActivator implements BundleActivator{

	// Gets executed when the bundle is started
	@Override
	public void start(BundleContext context) throws Exception {
		System.out.println("Starting org.mci.demo");
		
		try {
			
			// Get Service Reference for HeatingSensorService
			ServiceReference<?> sensorServiceRef = context.getServiceReference(HeatingSensorService.class.getName());
			if (sensorServiceRef == null) {
				throw new Exception("Could not get service reference for service HeatingSensorService");
			}

			// Get actual implementation of HeatingSensorService
			HeatingSensorService sensorService = (HeatingSensorService) context.getService(sensorServiceRef);
			if (sensorService == null) {
				throw new Exception("Could not get service HeatingSensorService");
			}
			
			// Call a method from HeatingSensorService
			double temp = sensorService.getSensorTemperature();
			System.out.println("Current Temperature: " + temp);
			
			
		} catch(Exception e) {
			System.err.println("Caught exception: " + e);
		}
	}

	// Gets executed when the bundle is stopped
	@Override
	public void stop(BundleContext context) throws Exception {
		System.out.println("Stopping org.mci.demo");
	}

}
