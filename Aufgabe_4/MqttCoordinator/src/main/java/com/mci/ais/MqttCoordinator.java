package com.mci.ais;

import org.eclipse.paho.client.mqttv3.*;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

public class MqttCoordinator implements MqttCallback {

    static private boolean shutdownFlag = false;

    public static class WorkerList
    {
        String workerId;
        String workerTopicInbox;
        String workerTopicOutbox;
        String workerStatus;
        String workerLastHeartbeat;
    }

    private static final SimpleDateFormat timestamp = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

    static public List<WorkerList> workerList = new ArrayList<>();
    public static void main(String[] args) throws MqttException {

        String id = UUID.randomUUID().toString();


        MqttClient client = new MqttClient("tcp://localhost:1883", id);
        client.connect();

        MqttCoordinator myapp = new MqttCoordinator();
        client.setCallback(myapp);

        client.subscribe("coordinator/mailbox/+");
        client.subscribe("coordinator/mailbox/LastHeartbeat/+");
        //client.subscribe("coordinator/heartbeat");
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

            try {
                for (WorkerList worker : workerList) {

                    System.out.println("Worker ID: " + worker.workerId + " Topic: " + worker.workerTopicInbox +
                            "LastHeartbeat/" + worker.workerId + " Status: " + worker.workerStatus + " Last Heartbeat: "
                            + worker.workerLastHeartbeat);
                    String alive = "Worker form Cordinator ! Are you still alive?";
                    MqttMessage call = new MqttMessage(alive.getBytes());
                    client.publish(worker.workerTopicInbox + "LastHeartbeat/" + worker.workerId, call);
                }
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

        if (topic.split("/")[0].equals("coordinator") && topic.split("/")[1].equals("mailbox") && message.toString().equals("subscribe")){
            WorkerList worker = new WorkerList();
            worker.workerId = topic.split("/")[3];
            worker.workerTopicInbox = "coordinator/mailbox/";
            worker.workerTopicOutbox = "worker/mailbox/";
            workerList.add(worker);
            System.out.println("Message received (Topic: " + topic + "): Subscriber ID: " + worker.workerId + " added to worker list. ");
        }

        for(WorkerList worker : workerList) {
            if (topic.equals(worker.workerTopicInbox + "LastHeartbeat/" + worker.workerId) && message.toString().equals("alive")) {
                worker.workerStatus = "alive";
                worker.workerLastHeartbeat = timestamp.format(System.currentTimeMillis());
            }
        }

    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        System.out.println("deliveryComplete(" + token + ") called");
    }
}