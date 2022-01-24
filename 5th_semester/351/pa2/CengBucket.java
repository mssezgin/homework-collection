import java.util.ArrayList;

public class CengBucket {

    public int localDepth = 0;
    public String prefix = "";
    public ArrayList<CengPoke> records = new ArrayList<>();

    // GUI-Based Methods
    // These methods are required by GUI to work properly.

    public int pokeCount() {
        // Return the pokemon count in the bucket.
        return records.size();
    }

    public CengPoke pokeAtIndex(int index) {
        // Return the corresponding pokemon at the index.
        if (index < CengPokeKeeper.getBucketSize()) {
            return records.get(index);
        } else {
            return null;
        }
    }

    public int getHashPrefix() {
        // Return hash prefix length.
        return localDepth;
    }

    public Boolean isVisited() {
        // TODO: Return whether the bucket is found while searching.
        return false;
    }

    // Own Methods

    public int getLocalDepth() {
        return localDepth;
    }

    public void setPrefix(String prefix) {
        this.prefix = prefix;
        this.localDepth = prefix.length();
    }

    public void addPoke(CengPoke poke) {
        records.add(poke);
    }

    public void print(String indent, int maxGlobalDepth) {
        System.out.print("{\n");
        System.out.print(indent + "\t\"hashLength\": " + localDepth + ",\n");
        System.out.print(indent + "\t\"pokes\": [");
        for (int i = 0; i < records.size(); i++) {
            if (i > 0) {
                System.out.print(",");
            }
            System.out.print("\n" + indent + "\t\t\"poke\": ");
            records.get(i).print(indent + "\t\t", maxGlobalDepth);
        }
        System.out.print("\n" + indent + "\t]\n");
        System.out.print(indent + "}");
    }
}
