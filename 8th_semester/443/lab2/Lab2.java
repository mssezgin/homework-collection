import java.util.HashMap;
import java.util.concurrent.*;
import java.util.concurrent.locks.*;



// Lab2
public class Lab2 {
    public static void main(String[] args) {
        int nScribes    = Integer.parseInt(args[0]);
        int nPens       = Integer.parseInt(args[1]);
        int nInkBottles = Integer.parseInt(args[2]);
        int nBooks      = Integer.parseInt(args[3]);

        Resources.addResource("pens", new Resource(nPens));
        Resources.addResource("inkBottles", new Resource(nInkBottles));
        Resources.addResource("books", new Resource(nBooks));

        ExecutorService executor = Executors.newCachedThreadPool();
        for (int scribeId = 1; scribeId <= nScribes; scribeId++) {
            executor.execute(new Scribe(scribeId));
        }
        executor.shutdown();
    }
}



// Resources
class Resources {
    private static HashMap<String, Resource> resources = new HashMap<>();

    public static void addResource(String name, Resource resource) {
        resources.put(name, resource);
    }

    public static Resource getResource(String name) {
        return resources.get(name);
    }

    public static void print() {
        System.out.println(resources);
    }
}



// Resource
class Resource {
    private int count;
    private Lock lock = new ReentrantLock(true);
    private Condition newResourceCond = lock.newCondition();

    public Resource(int count) {
        this.count = count;
    }

    public void get(int n, String logMessage) {
        lock.lock();
        try {
            while (count < n) {
                newResourceCond.await();
            }
            count -= n;
            log(logMessage);
        } catch (InterruptedException e) {
            System.err.println("InterruptedException in Resource.get(n)");
        } finally {
            lock.unlock();
        }
    }

    public void put(int n, String logMessage) {
        lock.lock();
        count += n;
        log(logMessage);
        newResourceCond.signalAll();
        lock.unlock();
    }

    public void log(String message) {
        System.out.println(message);
        // Resources.print();
    }

    @Override
    public String toString() {
        return String.valueOf(count);
    }
}



// Scribe
class Scribe implements Runnable {
    private int id;

    public Scribe(int id) {
        this.id = id;
    }

    @Override
    public void run() {
        String logMessage = "";

        while (true) {

            // get resources
            logMessage = "Scribe " + id + " takes a pen";
            Resources.getResource("pens").get(1, logMessage);

            logMessage = "Scribe " + id + " takes a bottle";
            Resources.getResource("inkBottles").get(1, logMessage);

            logMessage = "Scribe " + id + " takes a book";
            Resources.getResource("books").get(1, logMessage);


            // write a record
            System.out.println("Scribe " + id + " writes a record");


            // // sleep a little
            // try {
            //     System.out.println("Scribe " + id + " starts sleeping");
            //     Thread.sleep(50);
            //     System.out.println("Scribe " + id + " wakes up");
            // } catch (InterruptedException e) {
            //     System.err.println("InterruptedException in Scribe.run()");
            // }


            // put resources
            logMessage = "Scribe " + id + " puts the pen back";
            Resources.getResource("pens").put(1, logMessage);

            logMessage = "Scribe " + id + " puts the bottle back";
            Resources.getResource("inkBottles").put(1, logMessage);

            logMessage = "Scribe " + id + " puts the book back";
            Resources.getResource("books").put(1, logMessage);

        }
    }
}
