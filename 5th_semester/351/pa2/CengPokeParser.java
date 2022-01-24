import java.io.*;
import java.util.ArrayList;

public class CengPokeParser {

    public static ArrayList<CengPoke> parsePokeFile(String filename) {
        ArrayList<CengPoke> pokeList = new ArrayList<CengPoke>();

        // You need to parse the input file in order to use GUI tables.
        // Parse the input file, and convert them into CengPokes

        try {
            BufferedReader br = new BufferedReader(new FileReader(filename));
            String line;

            while ((line = br.readLine()) != null) {
                String[] tokens = line.split("\t", -1);
                if (tokens[0].toLowerCase().equals("poke")) {
                    pokeList.add(new CengPoke(
                            Integer.parseInt(tokens[1]),
                            tokens[2],
                            tokens[3],
                            tokens[4]
                    ));
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        return pokeList;
    }

    public static void startParsingCommandLine() throws IOException {
        // Start listening and parsing command line -System.in-.
        // There are 5 commands:
        // 1) quit : End the app. Print nothing, call nothing.
        // 2) add : Parse and create the poke, and call CengPokeKeeper.addPoke(newlyCreatedPoke).
        // 3) search : Parse the pokeKey, and call CengPokeKeeper.searchPoke(parsedKey).
        // 4) delete: Parse the pokeKey, and call CengPokeKeeper.removePoke(parsedKey).
        // 5) print : Print the whole hash table with the corresponding buckets, call CengPokeKeeper.printEverything().

        // Commands (quit, add, search, print) are case-insensitive.

        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        String line;
        boolean quitCalled = false;

        while (!quitCalled && (line = br.readLine()) != null) {
            String[] tokens = line.split("\t", -1);
            switch (tokens[0].toLowerCase()) {
                case "add":
                    CengPokeKeeper.addPoke(new CengPoke(
                            Integer.parseInt(tokens[1], 10),
                            tokens[2],
                            tokens[3],
                            tokens[4]
                    ));
                    break;
                case "search":
                    CengPokeKeeper.searchPoke(Integer.parseInt(tokens[1], 10));
                    break;
                case "delete":
                    CengPokeKeeper.deletePoke(Integer.parseInt(tokens[1], 10));
                    break;
                case "print":
                    CengPokeKeeper.printEverything();
                    break;
                case "quit":
                    quitCalled = true;
                    break;
                default:
                    break;
            }
        }
    }
}
