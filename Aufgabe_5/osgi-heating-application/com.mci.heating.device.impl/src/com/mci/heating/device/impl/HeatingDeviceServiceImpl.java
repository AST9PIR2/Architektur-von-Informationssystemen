package com.mci.heating.device.impl;

import com.mci.heating.api.HeatingDeviceService;
import org.osgi.service.component.annotations.Component;

@Component(
	immediate=true // Start the bundle as soon as all dependencies are satisfied
)
public class HeatingDeviceServiceImpl implements HeatingDeviceService{

	private static int level;

	// Sets the current heating level
	// This bundle does nothing, but a real implementation would connect to a heating system and set the value there
	@Override
	public void setHeatingLevel(int level) {
		HeatingDeviceServiceImpl.level = level;
	}

	// Gets the current heating level
	@Override
	public int getHeatingLevel() {
		return level;
	}

}
