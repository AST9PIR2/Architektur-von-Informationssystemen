package org.example;

import java.util.concurrent.ThreadLocalRandom;


public class WorkerThread implements Runnable {

    private final String command;

    public WorkerThread(String s) {
        this.command = s;
    }

    public static boolean isInside (double xPos, double yPos)
    {
        double distance = Math.sqrt((xPos * xPos) + (yPos * yPos));

        return (distance < 1.0);
    }

    @Override
    public void run() {
        //System.out.println(Thread.currentThread().getName() + " Start. Command = " + command);
        processCommand_new();
        //System.out.println(Thread.currentThread().getName() + " End.");
    }

    private void processCommand_old() {
        try {
            Thread.sleep(1);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void processCommand_new() {
        double xPos = (ThreadLocalRandom.current().nextDouble()) * 2 - 1.0;
        double yPos = (ThreadLocalRandom.current().nextDouble()) * 2 - 1.0;

        // Was the coordinate hitting the dart board?
        if (isInside(xPos, yPos))
        {
            Main.Globals.hits++;
            //System.out.println("HITS!");
        }
    }

    @Override
    public String toString() {
        return this.command;
    }
}
