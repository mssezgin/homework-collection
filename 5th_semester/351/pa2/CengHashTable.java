import java.util.ArrayList;

public class CengHashTable {

    public int globalDepth = 0;
    public int maxGlobalDepth = 0;
    public ArrayList<CengHashRow> hashRows = new ArrayList<>();

    public CengHashTable() {
        // Create a hash table with only 1 row.
        hashRows.add(new CengHashRow());
        int hashMod = CengPokeKeeper.getHashMod();
        while (hashMod > 1) {
            maxGlobalDepth++;
            hashMod = hashMod >> 1;
        }
    }

    public void deletePoke(Integer pokeKey) {
        int index = this.hashIndex(pokeKey);
        CengBucket bucket = hashRows.get(index).getBucket();

        for (int i = 0; i < bucket.records.size(); i++) {
            if (bucket.records.get(i).pokeKey().equals(pokeKey)) {
                bucket.records.remove(i);
                break;
            }
        }
        System.out.println("\"delete\": {");
        System.out.println("\t\"emptyBucketNum\": " + CengPokeKeeper.getBucketList().countEmptyBuckets());
        System.out.println("}");
    }

    public void addPoke(CengPoke poke) {
        int index = this.hashIndex(poke.pokeKey());
        CengBucket bucket = this.hashRows.get(index).getBucket();

        if (bucket.pokeCount() < CengPokeKeeper.getBucketSize()) {
            bucket.addPoke(poke);
        } else {
            if (bucket.getLocalDepth() == this.globalDepth) {
                if (this.globalDepth < this.maxGlobalDepth) {
                    this.doubleTable();
                } else {
                    return;
                }
            }
            ArrayList<CengPoke> copyOfRecords = (ArrayList<CengPoke>) bucket.records.clone();
            bucket.records.clear();
            String prefix = bucket.prefix;
            bucket.setPrefix(prefix + "0");
            CengBucket newBucket = new CengBucket();
            newBucket.setPrefix(prefix + "1");

            int newBucketIndex = CengPokeKeeper.getBucketList().addBucket(newBucket);
            int startRow = Integer.parseInt(newBucket.prefix + "0".repeat(this.globalDepth - newBucket.localDepth), 2);
            int endRow = Integer.parseInt(newBucket.prefix + "1".repeat(this.globalDepth - newBucket.localDepth), 2);
            for (int i = startRow; i <= endRow; i++) {
                this.hashRows.get(i).setBucketIndex(newBucketIndex);
            }
            for (int i = 0; i < copyOfRecords.size(); i++) {
                this.addPoke(copyOfRecords.get(i));
            }
            this.addPoke(poke);
        }
    }

    public void searchPoke(Integer pokeKey) {
        System.out.print("\"search\": ");
        System.out.print("{");
        int index = this.hashIndex(pokeKey);
        CengBucket bucket = hashRows.get(index).getBucket();

        boolean found = false;
        for (int i = 0; i < bucket.records.size(); i++) {
            if (bucket.records.get(i).pokeKey().equals(pokeKey)) {
                found = true;
                break;
            }
        }
        if (found) {
            int startRow;
            int endRow;
            if (this.globalDepth == 0) {
                startRow = endRow = 0;
            } else {
                startRow = Integer.parseInt(bucket.prefix + "0".repeat(this.globalDepth - bucket.localDepth), 2);
                endRow = Integer.parseInt(bucket.prefix + "1".repeat(this.globalDepth - bucket.localDepth), 2);
            }
            for (int i = startRow; i <= endRow; i++) {
                if (i > startRow) {
                    System.out.print(",");
                }
                System.out.print("\n\t\"row\": ");
                hashRows.get(i).print("\t", maxGlobalDepth);
            }
        }
        System.out.println("\n}");
    }

    public void print() {
        System.out.print("\"table\": ");
        System.out.print("{");
        for (int i = 0; i < hashRows.size(); i++) {
            if (i > 0) {
                System.out.print(",");
            }
            System.out.print("\n\t\"row\": ");
            hashRows.get(i).print("\t", maxGlobalDepth);
        }
        System.out.println("\n}");
    }

    // GUI-Based Methods
    // These methods are required by GUI to work properly.

    public int prefixBitCount() {
        // Return table's hash prefix length.
        return globalDepth;
    }

    public int rowCount() {
        // Return the count of HashRows in table.
        return hashRows.size();
    }

    public CengHashRow rowAtIndex(int index) {
        // Return corresponding hashRow at index.
        return hashRows.get(index);
    }

    // Own Methods

    private int hashIndex(Integer pokeKey) {
        int hashValue = (int) pokeKey % CengPokeKeeper.getHashMod();
        return hashValue >> (maxGlobalDepth - globalDepth);
    }

    private void doubleTable() {
        ArrayList<CengHashRow> newHashRows = new ArrayList<>();
        for (CengHashRow hashRow : this.hashRows) {
            newHashRows.add(new CengHashRow(hashRow.prefix + "0", hashRow.bucketIndex));
            newHashRows.add(new CengHashRow(hashRow.prefix + "1", hashRow.bucketIndex));
        }
        this.hashRows = newHashRows;
        this.globalDepth++;
    }

    /* int mask = prefixMask(globalDepth);
    int hashIndex = (hashValue & mask) >> (maxGlobalDepth - globalDepth);
    System.out.println("hashValue: " + hashValue + " mask: " + mask + " hashIndex: " + hashIndex); */
    /* private int prefixMask(int b) {
        int mask = 0;
        int i;
        for (i = 0; i < b; i++) {
            mask = mask << 1;
            mask++;
        }
        for (; i < maxGlobalDepth; i++) {
            mask = mask << 1;
        }
        return mask;
    } */
}
