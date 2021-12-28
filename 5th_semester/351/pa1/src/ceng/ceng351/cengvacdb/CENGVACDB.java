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
        String url = "jdbc:mysql://" + host + ":" + port + "/" + database + "?useSSL=false";
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
                        "userID INT, " +
                        "userName VARCHAR(30), " +
                        "age INT, " +
                        "address VARCHAR(150), " +
                        "password VARCHAR(30), " +
                        "status VARCHAR(15), " +
                        "PRIMARY KEY (userID)" +
                ");",
                "CREATE TABLE Vaccine(" +
                        "code INT, " +
                        "vaccinename VARCHAR(30), " +
                        "type VARCHAR(30), " +
                        "PRIMARY KEY (code)" +
                ");",
                // TODO: add ON DELETE, but where?
                // TODO: PRIMARY KEY (code, userID) is not correct
                "CREATE TABLE Vaccination(" +
                        "code INT, " +
                        "userID INT, " +
                        "dose INT, " +
                        "vacdate DATE, " +
                        "PRIMARY KEY (code, userID, dose), " +
                        "FOREIGN KEY (code) REFERENCES Vaccine(code) ON DELETE CASCADE, " +
                        "FOREIGN KEY (userID) REFERENCES User(userID)" +
                ");",
                "CREATE TABLE AllergicSideEffect(" +
                        "effectcode INT, " +
                        "effectname VARCHAR (50), " +
                        "PRIMARY KEY (effectcode)" +
                ");",
                "CREATE TABLE Seen(" +
                        "effectcode INT, " +
                        "code INT, " +
                        "userID INT, " +
                        "date DATE, " +
                        "degree VARCHAR(30), " +
                        "PRIMARY KEY (effectcode, code, userID), " +
                        "FOREIGN KEY (effectcode) REFERENCES AllergicSideEffect(effectcode), " +
                        "FOREIGN KEY (code) REFERENCES Vaccination(code) ON DELETE CASCADE, " +
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
                    System.out.println("DROP TABLE IF EXISTS" + tables[i] + ";");
                    st.executeUpdate("DROP TABLE IF EXISTS " + tables[i] + ";");
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
                                user.getUserID() + ", " +
                                "'" + user.getUserName() + "', " +
                                user.getAge() + ", " +
                                "'" + user.getAddress() + "', " +
                                "'" + user.getPassword() + "', " +
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

    @Override
    public int insertAllergicSideEffect(AllergicSideEffect[] sideEffects) {
        int numberOfSideEffectsAdded = 0;
        try {
            Statement st = connection.createStatement();
            for (AllergicSideEffect sideEffect : sideEffects) {
                String query = "INSERT INTO AllergicSideEffect(effectcode, effectname) " +
                        "VALUES(" +
                                sideEffect.getEffectCode() + ", " +
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
    public int insertVaccine(Vaccine[] vaccines) {
        int numberOfVaccinesAdded = 0;
        try {
            Statement st = connection.createStatement();
            for (Vaccine vaccine : vaccines) {
                String query = "INSERT INTO Vaccine(code, vaccinename, type) " +
                        "VALUES(" +
                        vaccine.getCode() + ", " +
                        "'" + vaccine.getVaccineName() + "', " +
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
    public int insertVaccination(Vaccination[] vaccinations) {
        int numberOfVaccinationsAdded = 0;
        try {
            Statement st = connection.createStatement();
            for (Vaccination vaccination : vaccinations) {
                String query = "INSERT INTO Vaccination(code, userID, dose, vacdate) " +
                        "VALUES(" +
                                vaccination.getCode() + ", " +
                                vaccination.getUserID() + ", " +
                                vaccination.getDose() + ", " +
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
                                seen.getEffectcode() + ", " +
                                seen.getCode() + ", " +
                                "'" + seen.getUserID() + "', " +
                                "'" + seen.getDate() + "', " +
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
        String query =
                "SELECT V.code, V.vaccinename, V.type " +
                "FROM Vaccine V " +
                "WHERE V.code NOT IN (" +
                    "SELECT DISTINCT C.code " +
                    "FROM Vaccination C" +
                ") " +
                "ORDER BY V.code ASC;";
        Vaccine[] vaccines = null;
        try {
            Statement st = connection.createStatement();
            ResultSet rs = st.executeQuery(query);
            int size = 0;
            if (rs.last()) {
                size = rs.getRow();
                rs.beforeFirst();
            }
            vaccines = new Vaccine[size];
            for (int i = 0; rs.next(); i++) {
                vaccines[i] = new Vaccine(
                        rs.getInt("code"),
                        rs.getString("vaccinename"),
                        rs.getString("type")
                );
            }
            rs.close();
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return vaccines;
    }

    @Override
    public QueryResult.UserIDuserNameAddressResult[] getVaccinatedUsersforTwoDosesByDate(String vacdate) {
        // TODO
        return new QueryResult.UserIDuserNameAddressResult[0];
    }

    @Override
    public Vaccine[] getTwoRecentVaccinesDoNotContainVac() {
        // TODO: use DISTINCT
        String query =
                "SELECT V.code, V.vaccinename, V.type " +
                "FROM Vaccine V, Vaccination C " +
                "WHERE V.code = C.code AND " +
                    "V.code IN (" +
                        "SELECT V1.code " +
                        "FROM Vaccine V1" +
                    ") AND " +
                    "V.code NOT IN (" +
                        "SELECT V2.code " +
                        "FROM Vaccine V2 " +
                        "WHERE V2.vaccinename LIKE '%vac%'" +
                    ") " +
                "ORDER BY C.vacdate DESC, V.code ASC " +
                "LIMIT 2;";
        Vaccine[] vaccines = null;
        try {
            Statement st = connection.createStatement();
            ResultSet rs = st.executeQuery(query);
            int size = 0;
            if (rs.last()) {
                size = rs.getRow();
                rs.beforeFirst();
            }
            vaccines = new Vaccine[size];
            for (int i = 0; rs.next(); i++) {
                vaccines[i] = new Vaccine(
                        rs.getInt("code"),
                        rs.getString("vaccinename"),
                        rs.getString("type")
                );
            }
            rs.close();
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return vaccines;
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
        String query =
                "SELECT U.userID, U.userName, U.address " +
                "FROM User U " +
                "WHERE " +
                    "U.userID IN (" +
                        "SELECT C.userID " +
                        "FROM (" +
                            "SELECT C2.userID, C2.code " +
                            "FROM Vaccination C2 " +
                            "WHERE C2.vacdate >= '2000-01-01' AND C2.vacdate <= '2021-12-31' " +
                            "GROUP BY C2.userID, C2.code" +
                        ") as C " +
                        "GROUP BY C.userID " +
                        "HAVING COUNT(*) >= 2" +
                    ") " +
                "ORDER BY U.userID ASC;";
        QueryResult.UserIDuserNameAddressResult[] userResults = null;
        try {
            Statement st = connection.createStatement();
            ResultSet rs = st.executeQuery(query);
            int size = 0;
            if (rs.last()) {
                size = rs.getRow();
                rs.beforeFirst();
            }
            userResults = new QueryResult.UserIDuserNameAddressResult[size];
            for (int i = 0; rs.next(); i++) {
                userResults[i] = new QueryResult.UserIDuserNameAddressResult(
                        rs.getString("userID"),
                        rs.getString("userName"),
                        rs.getString("address")
                );
            }
            rs.close();
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return userResults;
    }

    @Override
    public AllergicSideEffect[] getSideEffectsOfUserWhoHaveTwoDosesInLessThanTwentyDays() {
        String query =
                "SELECT A.effectcode, A.effectname " +
                "FROM AllergicSideEffect A " +
                "WHERE " +
                    "A.effectcode IN (" +
                        "SELECT S.effectcode " +
                        "FROM Seen S, Vaccination C1, Vaccination C2 " +
                        "WHERE " +
                            "S.userID = C1.userID AND " +
                            "S.userID = C2.userID AND " +
                            "S.code = C1.code AND " +
                            "S.code = C2.code AND " +
                            "C1.dose = 1 AND " +
                            "C2.dose = 2 AND " +
                            "DATEDIFF(C2.vacdate, C1.vacdate) < 20" +
                    ") " +
                "ORDER BY A.effectcode ASC;";
        AllergicSideEffect[] allergicSideEffects = null;
        try {
            Statement st = connection.createStatement();
            ResultSet rs = st.executeQuery(query);
            int size = 0;
            if (rs.last()) {
                size = rs.getRow();
                rs.beforeFirst();
            }
            allergicSideEffects = new AllergicSideEffect[size];
            for (int i = 0; rs.next(); i++) {
                allergicSideEffects[i] = new AllergicSideEffect(
                        rs.getInt("effectcode"),
                        rs.getString("effectname")
                );
            }
            rs.close();
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return allergicSideEffects;
    }

    @Override
    public double averageNumberofDosesofVaccinatedUserOverSixtyFiveYearsOld() {
        String query =
                "SELECT AVG(T.maxdose) as avgdose " +
                "FROM (" +
                    "SELECT C.code, C.userID, MAX(C.dose) as maxdose " +
                    "FROM Vaccination C, User U " +
                    "WHERE C.userID = U.userID AND U.age >= 65 " +
                    "GROUP BY C.code, C.userID" +
                ") as T;";
        double avgdose = 0;
        try {
            Statement st = connection.createStatement();
            ResultSet rs = st.executeQuery(query);
            if (rs.next()) {
                avgdose = rs.getInt("avgdose");
            }
            rs.close();
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return avgdose;
    }

    @Override
    public int updateStatusToEligible(String givendate) {
        String update =
                "UPDATE User U " +
                "SET U.status = 'Eligible' " +
                "WHERE U.status != 'Eligible' AND U.userID IN (" +
                    "SELECT C.userID " +
                    "FROM Vaccination C " +
                    "GROUP BY C.userID " +
                    "HAVING DATEDIFF(CURRENT_TIME(), MAX(C.vacdate)) >= 120" +
                ");";
        int numberOfRowsUpdated = 0;
        try {
            Statement st = connection.createStatement();
            numberOfRowsUpdated = st.executeUpdate(update);
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return numberOfRowsUpdated;
    }

    @Override
    public Vaccine deleteVaccine(String vaccineName) {
        String query =
                "SELECT V.code, V.vaccinename, V.type " +
                "FROM Vaccine V " +
                "WHERE V.vaccinename = 'vaccinename';";
        String delete =
                "DELETE FROM Vaccine V " +
                "WHERE V.vaccinename = 'vaccinename';";
        Vaccine vaccine = null;
        try {
            Statement st = connection.createStatement();
            ResultSet rs = st.executeQuery(query);
            if (rs.next()) {
                vaccine = new Vaccine(
                        rs.getInt("code"),
                        rs.getString("vaccinename"),
                        rs.getString("type")
                );
            }
            st.executeUpdate(delete);
            rs.close();
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return vaccine;
    }
}
