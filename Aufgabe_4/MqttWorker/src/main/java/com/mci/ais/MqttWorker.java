package com.mci.ais;

import org.eclipse.paho.client.mqttv3.*;

import java.util.UUID;

public class MqttWorker implements MqttCallback {

    static private boolean shutdownFlag = false;
    public static void main(String[] args) throws MqttException {

        String id = UUID.randomUUID().toString();


        MqttClient client = new MqttClient("tcp://localhost:1883", id);
        client.connect();

        MqttWorker myapp = new MqttWorker();
        client.setCallback(myapp);

        client.subscribe("worker/mailbox/" + id);
        client.subscribe("worker/mailbox/LastHeartbeat/" + id);
        client.subscribe("application/shutdown");

        String subscribe = "subscribe";

        MqttMessage subscribemsg = new MqttMessage(subscribe.getBytes());
        client.publish("coordinator/mailbox/LastHeartbeat/" + id, subscribemsg);

        while(!shutdownFlag) {

            String content = "alive";
            MqttMessage msg = new MqttMessage(content.getBytes());
            client.publish("coordinator/mailbox/LastHeartbeat/" + id, msg);
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