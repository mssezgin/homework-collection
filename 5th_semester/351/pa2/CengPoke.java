public class CengPoke {

    private Integer pokeKey;
    private String pokeName;
    private String pokePower;
    private String pokeType;

    public CengPoke(Integer pokeKey, String pokeName, String pokePower, String pokeType) {
        this.pokeKey = pokeKey;
        this.pokeName = pokeName;
        this.pokePower = pokePower;
        this.pokeType = pokeType;
    }

    // Getters

    public Integer pokeKey() {
        return pokeKey;
    }

    public String pokeName() {
        return pokeName;
    }

    public String pokePower() {
        return pokePower;
    }

    public String pokeType() {
        return pokeType;
    }

    // GUI method - do not modify
    public String fullName() {
        return "" + pokeKey() + "\t" + pokeName() + "\t" + pokePower() + "\t" + pokeType;
    }

    // Own Methods

    public void print(String indent, int maxGlobalDepth) {
        System.out.print("{\n");
        String hash = Integer.toBinaryString(pokeKey % CengPokeKeeper.getHashMod());
        hash = "0".repeat(maxGlobalDepth - hash.length()) + hash;
        System.out.print(indent + "\t\"hash\": " + hash + ",\n");
        System.out.print(indent + "\t\"pokeKey\": " + pokeKey + ",\n");
        System.out.print(indent + "\t\"pokeName\": " + pokeName + ",\n");
        System.out.print(indent + "\t\"pokePower\": " + pokePower + ",\n");
        System.out.print(indent + "\t\"pokeType\": " + pokeType + "\n");
        System.out.print(indent + "}");
    }
}
