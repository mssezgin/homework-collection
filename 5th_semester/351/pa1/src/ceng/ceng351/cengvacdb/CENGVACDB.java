package ceng.ceng351.cengvacdb;

import java.sql.*;

public class CENGVACDB implements ICENGVACDB {
    private static String username = "e2380863";
    private static String password = "f*4ZtsHSlabU";
    private static String host = "144.122.71.121";
    private static int port = 8080;
    private static String database = "db2380863";
    private static Connection connection = null;

    @Override
    public void initialize() {
        String url = "jdbc:mysql://" + host + ":" + port + "/" + database;
        try {
            connection = DriverManager.getConnection(url, username, password);
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    @Override
    public int createTables() {
        String[] tables = {
                "CREATE TABLE User(" +
                        "userId INT," +
                        "userName VARCHAR(30)," +
                        "age INT," +
                        "address VARCHAR(150)," +
                        "password VARCHAR(30)," +
                        "status VARCHAR(10)," +
                        "PRIMARY KEY (userID)" +
                ");",
                "CREATE TABLE Vaccine(" +
                        "code INT," +
                        "vaccinename VARCHAR(30)," +
                        "type VARCHAR(30)," +
                        "PRIMARY KEY (code)" +
                ");",
                "CREATE TABLE Vaccination(" +
                        "code INT," +
                        "userID INT," +
                        "dose INT," +
                        "vacdate DATE," +
                        "PRIMARY KEY (code, userID)," +
                        "FOREIGN KEY (code) REFERENCES Vaccine(code)," +
                        "FOREIGN KEY (userID) REFERENCES User(userID)" +
                ");",
                "CREATE TABLE AllergicSideEffect(" +
                        "effectcode INT," +
                        "effectname VARCHAR (50)," +
                        "PRIMARY KEY (effectcode)," +
                ");",
                "CREATE TABLE Seen(" +
                        "effectcode INT," +
                        "code INT," +
                        "userID INT," +
                        "date DATE," +
                        "degree VARCHAR(30)," +
                        "PRIMARY KEY (effectcode, code, userID)," +
                        "FOREIGN KEY (effectcode) REFERENCES AllergicSideEffect(effectcode)," +
                        "FOREIGN KEY (code) REFERENCES Vaccination(code)," +
                        "FOREIGN KEY (userID) REFERENCES User(userID)" +
                ");"
        };
        int numberOfTablesCreated = 0;
        try {
            Statement st = connection.createStatement();
            for (int i = 0; i < tables.length; i++) {
                try {
                    st.executeQuery(tables[i]);
                    numberOfTablesCreated++;
                } catch (SQLException ignored) { }
            }
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return numberOfTablesCreated;
    }

    @Override
    public int dropTables() {
        String[] tables = { "Seen", "AllergicSideEffect", "Vaccination", "Vaccine", "User" };
        int numberOfTablesDropped = 0;
        try {
            Statement st = connection.createStatement();
            for (int i = 0; i < tables.length; i++) {
                try {
                    st.executeQuery("DROP TABLE " + tables[i] + ";");
                    numberOfTablesDropped++;
                } catch (SQLException ignored) { }
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return numberOfTablesDropped;
    }

    public int insertVaccine(Vaccine[] v) { return 0; }

    @Override
    public int insertUser(User[] users) {
        return 0;
    }

    @Override
    public int insertAllergicSideEffect(AllergicSideEffect[] sideEffects) {
        return 0;
    }

    @Override
    public int insertVaccination(Vaccination[] vaccinations) {
        return 0;
    }

    @Override
    public int insertSeen(Seen[] seens) {
        return 0;
    }

    @Override
    public Vaccine[] getVaccinesNotAppliedAnyUser() {
        return new Vaccine[0];
    }

    @Override
    public QueryResult.UserIDuserNameAddressResult[] getVaccinatedUsersforTwoDosesByDate(String vacdate) {
        return new QueryResult.UserIDuserNameAddressResult[0];
    }

    @Override
    public Vaccine[] getTwoRecentVaccinesDoNotContainVac() {
        return new Vaccine[0];
    }

    @Override
    public QueryResult.UserIDuserNameAddressResult[] getUsersAtHasLeastTwoDoseAtMostOneSideEffect() {
        return new QueryResult.UserIDuserNameAddressResult[0];
    }

    @Override
    public QueryResult.UserIDuserNameAddressResult[] getVaccinatedUsersWithAllVaccinesCanCauseGivenSideEffect(String effectname) {
        return new QueryResult.UserIDuserNameAddressResult[0];
    }

    @Override
    public QueryResult.UserIDuserNameAddressResult[] getUsersWithAtLeastTwoDifferentVaccineTypeByGivenInterval(String startdate, String enddate) {
        return new QueryResult.UserIDuserNameAddressResult[0];
    }

    @Override
    public AllergicSideEffect[] getSideEffectsOfUserWhoHaveTwoDosesInLessThanTwentyDays() {
        return new AllergicSideEffect[0];
    }

    @Override
    public double averageNumberofDosesofVaccinatedUserOverSixtyFiveYearsOld() {
        return 0;
    }

    @Override
    public int updateStatusToEligible(String givendate) {
        return 0;
    }

    @Override
    public Vaccine deleteVaccine(String vaccineName) {
        return null;
    }
}
