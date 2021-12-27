package ceng.ceng351.cengvacdb;

import java.sql.*;

public class CENGVACDB implements ICENGVACDB {
    private String username = "e2380863";
    private String password = "f*4ZtsHSlabU";
    private String host = "144.122.71.121";
    private int port = 8080;
    private String database = "db2380863";
    private Connection connection = null;

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
        // TODO: add ON DELETE, but where?
        // TODO: User.status length of 10 is not sufficient
        String[] tables = {
                "CREATE TABLE User(" +
                        "userID INT," +
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
                        "FOREIGN KEY (code) REFERENCES Vaccine(code) ON DELETE CASCADE," +
                        "FOREIGN KEY (userID) REFERENCES User(userID)" +
                ");",
                "CREATE TABLE AllergicSideEffect(" +
                        "effectcode INT," +
                        "effectname VARCHAR (50)," +
                        "PRIMARY KEY (effectcode)" +
                ");",
                "CREATE TABLE Seen(" +
                        "effectcode INT," +
                        "code INT," +
                        "userID INT," +
                        "date DATE," +
                        "degree VARCHAR(30)," +
                        "PRIMARY KEY (effectcode, code, userID)," +
                        "FOREIGN KEY (effectcode) REFERENCES AllergicSideEffect(effectcode)," +
                        "FOREIGN KEY (code) REFERENCES Vaccination(code) ON DELETE CASCADE," +
                        "FOREIGN KEY (userID) REFERENCES User(userID)" +
                ");"
        };
        int numberOfTablesCreated = 0;
        try {
            Statement st = connection.createStatement();
            for (int i = 0; i < tables.length; i++) {
                try {
                    // TODO: delete this
                    System.out.println(tables[i]);
                    st.executeUpdate(tables[i]);
                    numberOfTablesCreated++;
                } catch (SQLException ignored) {
                    // TODO: delete this
                    System.out.println("exception");
                }
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
                    // TODO: delete this
                    System.out.println("DROP TABLE " + tables[i] + ";");
                    st.executeUpdate("DROP TABLE " + tables[i] + ";");
                    numberOfTablesDropped++;
                } catch (SQLException ignored) {
                    // TODO: delete this
                     System.out.println("exception");
                }
            }
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return numberOfTablesDropped;
    }

    @Override
    public int insertUser(User[] users) {
        int numberOfUsersAdded = 0;
        try {
            Statement st = connection.createStatement();
            for (User user : users) {
                String query = "INSERT INTO User(userID, userName, age, address, password, status) " +
                        "VALUES(" +
                                user.getUserID() + "," +
                                "'" + user.getUserName() + "'," +
                                user.getAge() + "," +
                                "'" + user.getAddress() + "'," +
                                "'" + user.getPassword() + "'," +
                                "'" + user.getStatus() + "'" +
                        ");";
                // TODO: delete this
                System.out.println(query);
                try {
                    st.executeUpdate(query);
                    numberOfUsersAdded++;
                } catch (SQLException ignored) {
                    // TODO: delete this
                    System.out.println("exception");
                }
            }
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return numberOfUsersAdded;
    }

    // TODO: this function is not in the interface
    public int insertVaccine(Vaccine[] vaccines) {
        int numberOfVaccinesAdded = 0;
        try {
            Statement st = connection.createStatement();
            for (Vaccine vaccine : vaccines) {
                String query = "INSERT INTO Vaccine(code, vaccinename, type) " +
                        "VALUES(" +
                                vaccine.getCode() + "," +
                                "'" + vaccine.getVaccineName() + "'," +
                                "'" + vaccine.getType() + "'" +
                        ");";
                // TODO: delete this
                System.out.println(query);
                try {
                    st.executeUpdate(query);
                    numberOfVaccinesAdded++;
                } catch (SQLException ignored) {
                    // TODO: delete this
                    System.out.println("exception");
                }
            }
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return numberOfVaccinesAdded;
    }

    @Override
    public int insertAllergicSideEffect(AllergicSideEffect[] sideEffects) {
        int numberOfSideEffectsAdded = 0;
        try {
            Statement st = connection.createStatement();
            for (AllergicSideEffect sideEffect : sideEffects) {
                String query = "INSERT INTO AllergicSideEffect(effectcode, effectname) " +
                        "VALUES(" +
                                sideEffect.getEffectCode() + "," +
                                "'" + sideEffect.getEffectName() + "'" +
                        ");";
                // TODO: delete this
                System.out.println(query);
                try {
                    st.executeUpdate(query);
                    numberOfSideEffectsAdded++;
                } catch (SQLException ignored) {
                    // TODO: delete this
                    System.out.println("exception");
                }
            }
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return numberOfSideEffectsAdded;
    }

    @Override
    public int insertVaccination(Vaccination[] vaccinations) {
        int numberOfVaccinationsAdded = 0;
        try {
            Statement st = connection.createStatement();
            for (Vaccination vaccination : vaccinations) {
                String query = "INSERT INTO Vaccination(code, userID, dose, vacdate) " +
                        "VALUES(" +
                                vaccination.getCode() + "," +
                                vaccination.getUserID() + "," +
                                vaccination.getDose() + "," +
                                "'" + vaccination.getVacdate() + "'" +
                        ");";
                // TODO: delete this
                System.out.println(query);
                try {
                    st.executeUpdate(query);
                    numberOfVaccinationsAdded++;
                } catch (SQLException ignored) {
                    // TODO: delete this
                    System.out.println("exception");
                }
            }
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return numberOfVaccinationsAdded;
    }

    @Override
    public int insertSeen(Seen[] seens) {
        int numberOfSeensAdded = 0;
        try {
            Statement st = connection.createStatement();
            for (Seen seen : seens) {
                String query = "INSERT INTO Seen(effectcode, code, userID, date, degree) " +
                        "VALUES(" +
                                seen.getEffectcode() + "," +
                                seen.getCode() + "," +
                                "'" + seen.getUserID() + "'," +
                                "'" + seen.getDate() + "'," +
                                "'" + seen.getDegree() + "'" +
                        ");";
                // TODO: delete this
                System.out.println(query);
                try {
                    st.executeUpdate(query);
                    numberOfSeensAdded++;
                } catch (SQLException ignored) {
                    // TODO: delete this
                    System.out.println("exception");
                }
            }
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return numberOfSeensAdded;
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
