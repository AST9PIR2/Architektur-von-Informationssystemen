package com.mci.ais;

import org.eclipse.paho.client.mqttv3.*;

import java.util.UUID;

public class MqttApplication implements MqttCallback {

    static private boolean shutdownFlag = false;
    public static void main(String[] args) throws MqttException {

        String id = UUID.randomUUID().toString();


        MqttClient client = new MqttClient("tcp://localhost:1883", id);
        client.connect();

        MqttApplication myapp = new MqttApplication();
        client.setCallback(myapp);

        client.subscribe("mytopic01/bla");
        client.subscribe("mytopic02/+");
        client.subscribe("mytopic03/#");
        client.subscribe("application/shutdown");

        int counter = 0;

        while(!shutdownFlag) {

            String content = "Hello World!" + counter;
            counter ++;
            MqttMessage msg = new MqttMessage(content.getBytes());
            client.publish("mytopic04", msg);
            try {
                Thread.sleep(5000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        client.disconnect();
        client.close();
        System.out.println("Application finished");

    }

    @Override
    public void connectionLost(Throwable cause) {
        System.out.println("Connection to Mqtt broker lost!" + cause);
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        System.out.println("Message received (Topic: " + topic + "): " + message);

        if (topic.equals("application/shutdown") && message.toString().equals("exit")) {
            shutdownFlag = true;
        }

    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        System.out.println("deliveryComplete(" + token + ") called");
    }
}