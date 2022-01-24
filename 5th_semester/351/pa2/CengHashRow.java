public class CengHashRow {

    public String prefix = "";
    public int bucketIndex = 0;

    // GUI-Based Methods
    // These methods are required by GUI to work properly.

    public String hashPrefix() {
        // Return row's hash prefix (such as 0, 01, 010, ...)
        return prefix;
    }

    public CengBucket getBucket() {
        // Return the bucket that the row points at.
        return CengPokeKeeper.getBucketList().bucketAtIndex(bucketIndex);
    }

    public boolean isVisited() {
        // TODO: Return whether the row is used while searching.
        return false;
    }

    // Own Methods

    public CengHashRow() { }

    public CengHashRow(String prefix, int bucketIndex) {
        this.prefix = prefix;
        this.bucketIndex = bucketIndex;
    }

    public void setBucketIndex(int bucketIndex) {
        this.bucketIndex = bucketIndex;
    }

    public void print(String indent, int maxGlobalDepth) {
        System.out.print("{\n");
        System.out.print(indent + "\t\"hashPref\": " + (prefix.equals("") ? "0" : prefix) + ",\n");
        System.out.print(indent + "\t\"bucket\": ");
        getBucket().print(indent + "\t", maxGlobalDepth);
        System.out.print("\n" + indent + "}");
    }
}
