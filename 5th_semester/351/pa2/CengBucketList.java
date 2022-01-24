import java.util.ArrayList;

public class CengBucketList {

    public ArrayList<CengBucket> bucketList = new ArrayList<>();

    public CengBucketList() {
        // Empty Implementation
        bucketList.add(new CengBucket());
    }

    public void deletePoke(Integer pokeKey) {
        // TODO: Empty Implementation
    }

    public void addPoke(CengPoke poke) {
        // TODO: Empty Implementation
    }

    public void searchPoke(Integer pokeKey) {
        // TODO: Empty Implementation
    }

    public void print() {
        // TODO: Empty Implementation
    }

    // GUI-Based Methods
    // These methods are required by GUI to work properly.

    public int bucketCount() {
        // Return all bucket count.
        return bucketList.size();
    }

    public CengBucket bucketAtIndex(int index) {
        // Return corresponding bucket at index.
        return bucketList.get(index);
    }

    // Own Methods

    public int addBucket(CengBucket bucket) {
        bucketList.add(bucket);
        return bucketList.size() - 1;
    }

    public int countEmptyBuckets() {
        int count = 0;
        for (CengBucket bucket : bucketList) {
            if (bucket.records.size() == 0) {
                count++;
            }
        }
        return count;
    }
}
