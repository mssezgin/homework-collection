import javax.print.attribute.standard.NumberOfInterveningJobs;
import java.awt.*;
import java.util.concurrent.ThreadLocalRandom;
import java.util.List;
import java.util.ArrayList;

public class Common {
    private static final String title = "Economics 101";
    private static final Font font =new Font("Verdana", Font.BOLD, 20);
    private static final int windowWidth = 1650;
    private static final int windowHeight = 1000;

    private static final int entityDiameter = 20; // diameter of drawn entity (customer or store)

    private static final int storeNo = 10; // number of stores in the simulation
    private static final int customerNo = 10; // number of customers
    private static final int salespersonNo = 5;

    private static final int stockReplenishmentFrequency = 3000; // number of stepAllEntities calls before replenishing all stores

    private static final int foodBottomPrice = 20;
    private static final int foodCeilingPrice = 50;
    private static final int electronicsBottomPrice = 200;
    private static final int electronicsCeilingPrice = 2000;
    private static final int LuxuryBottomPrice = 5000;
    private static final int LuxuryCeilingPrice = 10000;

    private static final int minimumShoppingListLength = 5;
    private static final int maximumShoppingListLength = 10;

    private static final int stockStorageMin = 15; // minimum size of storage available for a store
    private static final int stockStorageMax = 25; // maximum number of storage available for a store

    private static final int salespersonStorageMin = 2;
    private static final int salespersonStorageMax = 5;

    private static final int customerMovementSpeed = 2;
    private static final int supplierMovementSpeed = 3;
    private static final int salespersonMovementSpeed = 3;
    private static final int salespersonMoveInterval = 1000;

    public static String getTitle() { return title; }

    public static int getWindowWidth() { return windowWidth; }

    public static int getWindowHeight() { return windowHeight; }

    public static int getEntityDiameter(){ return entityDiameter;}

    public static Font getFont() { return font; }

    public static int getCustomerNo() { return customerNo; }

    public static int getCustomerMovementSpeed() { return customerMovementSpeed; }

    public static int getSupplierMovementSpeed() { return supplierMovementSpeed; }

    public static int getSalespersonMovementSpeed() { return salespersonMovementSpeed; }

    public static int getSalespersonMoveInterval() { return salespersonMoveInterval; }

    private static final List<Store> storeList = new ArrayList<>();
    private static final List<Customer> customerList = new ArrayList<>();
    // TODO: create supplier reference
    // private static Supplier supplier = null;
    private static final List<Salesperson> salespersonList = new ArrayList<>();

    // TODO: delete replenish functionality
    private static int lastReplenishment = 0;
    public static List<Store> getStoreList() {
        return storeList;
    }

    public static List<Customer> getCustomerList() {
        return customerList;
    }

    public static List<Salesperson> getSalespersonList() {
        return salespersonList;
    }

    public static Product chooseProduct() {
        switch (randInt(0, 2)) {
            case 0:
                return Product.Food;
            case 1:
                return Product.Electronics;
            default:
                return Product.Luxury;
        }
    }

    public static Customer createCustomer() {
        double x = randInt(0, windowWidth - 2 * entityDiameter);
        double y = randInt(0, windowHeight - 2 * entityDiameter);
        Customer.Strategy strategy;
        switch (randInt(0, 2)) {
            case 0:
                strategy = Customer.Strategy.Cheapest;
                break;
            case 1:
                strategy = Customer.Strategy.Closest;
                break;
            default:
                strategy = Customer.Strategy.Travelling;
        }
        ArrayList<Product> shoppingList = new ArrayList<Product>();
        for (int i = 0; i < randInt(minimumShoppingListLength, maximumShoppingListLength); i++) {
            shoppingList.add(chooseProduct());
        }

        return new Customer(x, y, strategy, shoppingList);
    }

    private static int currentStoreType = 0;
    public static Store createStore() {
        double x = randInt(0, windowWidth - 2 * entityDiameter);
        double y = randInt(0, windowHeight - 2 * entityDiameter);
        int maxStock = randInt(stockStorageMin, stockStorageMax);
        int price;
        Product type;
        switch (currentStoreType) {
            case 0:
                type = Product.Food;
                price = randInt(foodBottomPrice, foodCeilingPrice);
                break;
            case 1:
                type = Product.Electronics;
                price = randInt(electronicsBottomPrice, electronicsCeilingPrice);
                break;
            default:
                type = Product.Luxury;
                price = randInt(LuxuryBottomPrice, LuxuryCeilingPrice);
                break;

        }
        currentStoreType = (currentStoreType + 1) % 3;
        return new Store(x, y, type, price, maxStock);
    }

    public static Salesperson createSalesperson() {
        double x = randInt(0, windowWidth - 2 * entityDiameter);
        double y = randInt(0, windowHeight - 2 * entityDiameter);
        int maxStock = randInt(salespersonStorageMin, salespersonStorageMax);
        Product type;
        switch (randInt(0, 2)) {
            case 0:
                type = Product.Food;
                break;
            case 1:
                type = Product.Electronics;
                break;
            case 2:
                type = Product.Luxury;
                break;
            default:
                type = null;
                break;
        }
        Salesperson.Strategy strategy;
        switch (randInt(0, 1)) {
            case 0:
                strategy = Salesperson.Strategy.Random;
                break;
            case 1:
                strategy = Salesperson.Strategy.MiddleGround;
                break;
            default:
                strategy = null;
                break;
        }
        return new Salesperson(x, y, type, maxStock, strategy);
    }

    public static int randInt(int min, int max) {
        return ThreadLocalRandom.current().nextInt(min, max + 1);
    }

    static {
        for (int i = 0; i < storeNo; i++) {
            storeList.add(createStore());
        }

        for (int i = 0; i < customerNo; i++) {
            customerList.add(createCustomer());
        }

        // TODO: create supplier

        for (int i = 0; i < salespersonNo; i++) {
            salespersonList.add(createSalesperson());
        }
    }

    public static void stepAllEntities() {
        // TODO: create or remove salesperson
        for (int i = 0; i < customerNo; i++) {
            Customer c = customerList.get(i);
            if (c.getPosition().getIntX() < -2 * entityDiameter || c.getPosition().getIntX() > windowWidth ||
                c.getPosition().getIntY() < -2 * entityDiameter || c.getPosition().getY() > windowHeight) {
                customerList.set(i, createCustomer());
            }
        }

        for(Store s : storeList) {
            s.step();
        }
        for(Customer c : customerList) {
            c.step();
        }
        // TODO: step supplier
        for(Salesperson s : salespersonList) {
            s.step();
        }

        // TODO: delete replenish functionality
        lastReplenishment += 1;
        if (lastReplenishment == stockReplenishmentFrequency){
            lastReplenishment = 0;
            for (Store s:storeList) {
                s.replenish();
            }
        }
    }
}
