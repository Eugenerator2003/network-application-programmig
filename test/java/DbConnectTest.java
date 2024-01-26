import com.example.laba8.DL.DriverProvider;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import java.sql.Connection;
import java.sql.DriverManager;

public class DbConnectTest {
    @Test
    public void DbConnectTest() throws Exception {
        DriverProvider.setDriver("com.mysql.jdbc.Driver");
        Connection connection =  DriverManager.getConnection("jdbc:mysql://localhost:3306/laba8",
                                                            "root",
                                                        "zalupa228");
        Assertions.assertNotNull(connection);
        System.out.println("Connected success");
    }
}
