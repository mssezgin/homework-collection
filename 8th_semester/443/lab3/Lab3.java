import java.io.*;
import java.util.*;
import java.util.stream.*;
import java.util.function.*;

public class Lab3 {
    public static void main(String[] args) {
        String csvFilename = args[0];
        int questionNumber = Integer.parseInt(args[1]);

        switch (questionNumber) {
            case 1:
                question1(csvFilename);
                break;
            case 2:
                question2(csvFilename);
                break;
            case 3:
                question3(csvFilename);
                break;
            case 4:
                question4(csvFilename);
                break;
            default:
                break;
        }
    }

    public static Stream<CustomerPurchase> parseCsv(Stream<String> lines) {
        return lines.skip(1).map(e -> e.split(",", -1)).map(CustomerPurchase::new);
    }

    public static void question1(String csvFilename) {
        try (BufferedReader br= new BufferedReader(new InputStreamReader(new FileInputStream(new File(csvFilename))))) {
            Stream<String> lines = br.lines();
            Stream<CustomerPurchase> cps = parseCsv(lines);
            cps
                .filter(e ->  e.date[2] == 15 && e.date[0] >= 1980 && e.date[0] <= 1990)
                .map(e -> {
                    long[] purchaseCount = new long[5];
                    for (int i = 0; i < 5; i++) {
                        purchaseCount[i] = (e.purchases[i] != null) ? 1 : 0;
                    }
                    return purchaseCount;
                })
                .reduce((e1, e2) -> {
                    for (int i = 0; i < 5; i++) {
                        e1[i] += e2[i];
                    }
                    return e1;
                })
                .ifPresent(e -> {
                    int maxIndex = 0;
                    for (int i = 0; i < 5; i++) {
                        if (e[i] > e[maxIndex]) {
                            maxIndex = i;
                        }
                    }
                    System.out.println((char) ('A' + maxIndex));
                });
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void question2(String csvFilename) {
        try (BufferedReader br= new BufferedReader(new InputStreamReader(new FileInputStream(new File(csvFilename))))) {
            Stream<String> lines = br.lines();
            Stream<CustomerPurchase> cps = parseCsv(lines);
            long result = cps
                .filter(e -> e.purchases[0] != null || e.purchases[3] != null)
                .filter(e -> e.numberOfProductsPurchased() >= 3)
                .count();
            System.out.println(result);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void question3(String csvFilename) {
        try (BufferedReader br= new BufferedReader(new InputStreamReader(new FileInputStream(new File(csvFilename))))) {
            Stream<String> lines = br.lines();
            Stream<CustomerPurchase> cps = parseCsv(lines);
            long result = cps
                .filter(e -> e.age >= 55)
                .filter(e -> e.date[0] >= 1995)
                .mapToInt(CustomerPurchase::numberOfProductsPurchased)
                .sum();
            System.out.println(result);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void question4(String csvFilename) {
        try (BufferedReader br= new BufferedReader(new InputStreamReader(new FileInputStream(new File(csvFilename))))) {
            Stream<String> lines = br.lines();
            Stream<CustomerPurchase> cps = parseCsv(lines);

            // I am reading the stream of file only once and grouping the lines by the year.
            Map<Integer, List<CustomerPurchase>> yearGroup = cps
                .filter(e -> e.purchases[1] != null)
                .collect(Collectors.groupingBy(e -> e.date[0]));

            // Here, I am using two stream to find min and max values of total purchase.
            // But, this stream is the stream of key value pairs of grouped lines.
            // I could have use sort function, in which case i would be reading this stream once.
            yearGroup.entrySet().stream()
                .min((e1, e2) -> Integer.compare(e1.getValue().size(), e2.getValue().size()))
                .ifPresent(e -> System.out.println(e.getKey()));
            yearGroup.entrySet().stream()
                .max((e1, e2) -> Integer.compare(e1.getValue().size(), e2.getValue().size()))
                .ifPresent(e -> System.out.println(e.getKey()));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

class CustomerPurchase {
    public int age;
    public int[] date;
    public Double[] purchases;

    public CustomerPurchase(String[] values) {
        age = Integer.parseInt(values[0]);
        String[] dateString = values[1].split("-", 3);
        date = new int[3];
        for (int i = 0; i < dateString.length; i++) {
            date[i] = Integer.parseInt(dateString[i]);
        }
        purchases = new Double[5];
        for (int i = 0; i < 5; i++) {
            if (!values[i + 2].equals("")) {
                purchases[i] = Double.valueOf(values[i + 2]);
            }
        }
    }

    public int numberOfProductsPurchased() {
        int purchaseCount = 0;
        for (int i = 0; i < purchases.length; i++) {
            if (purchases[i] != null) {
                purchaseCount++;
            }
        }
        return purchaseCount;
    }
}
