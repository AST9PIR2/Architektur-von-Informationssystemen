package org.example;

import java.util.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadLocalRandom;
// This program approximates PI using the Monte Carlo method.
//https://www.coderslexicon.com/a-slice-of-pi-using-the-monte-carlo-method-in-java/
//https://www.geeksforgeeks.org/estimating-value-pi-using-monte-carlo/
//https://stackoverflow.com/questions/1770010/how-do-i-measure-time-elapsed-in-java
//https://www.baeldung.com/java-thread-local-random
//https://www.baeldung.com/java-executor-service-tutorial
//https://www.baeldung.com/thread-pool-java-and-guava
//https://www.digitalocean.com/community/tutorials/threadpoolexecutor-java-thread-pool-example-executorservice

// Print a very basic program description and ask for number of throws

import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.RejectedExecutionHandler;
import java.util.concurrent.ThreadPoolExecutor;

import static org.example.WorkerThread.isInside;


public class Main {

    public static class Globals {
        public static int hits = 0;
    }



// Calculates PI based on the number of throws versus misses
public static double computePI (long numThrows)
{

    double PI = 0;
    int threads = 16;
    int divider = (int) (numThrows/threads);
    ExecutorService executor = Executors.newFixedThreadPool(divider);


    /*
    for (int i = 1; i <= numThrows; i++)
    {
        // Create a random coordinate result to test
        double xPos = (ThreadLocalRandom.current().nextDouble()) * 2 - 1.0;
        double yPos = (ThreadLocalRandom.current().nextDouble()) * 2 - 1.0;

        // Was the coordinate hitting the dart board?
        if (isInside(xPos, yPos))
        {
            Globals.hits++;
        }
    }*/



    for (long i = 1; i <= divider; i++) {
            WorkerThread worker = new WorkerThread("" + i);
            executor.execute(worker);

    }
    executor.shutdown();
    while (!executor.isTerminated()) {
    }
    System.out.println("Finished all threads");

    // Use Monte Carlo method formula
    PI = (4.0 * (Globals.hits/ (double) numThrows));

    return PI;
}

    public static void main(String[] args) {
        long numThrows = 1000_000_000L;
        System.out.println("This program approximates PI using the Monte Carlo method.");
        System.out.println("It simulates throwing " + numThrows + " darts at a dartboard.");
        long startTime = System.currentTimeMillis();
        double PI = computePI(numThrows);
        long estimatedTime = System.currentTimeMillis() - startTime;

        // Determine the difference from the PI constant defined in Math
        double Difference = PI - Math.PI;

        // Print out the total results of our trials
        System.out.println("Number of throws = " + numThrows + ", Computed PI = " + PI + ", Difference = " + Difference + ", Time = " + estimatedTime + "ms");
    }
}