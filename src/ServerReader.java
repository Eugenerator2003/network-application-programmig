import java.io.*;
import java.net.MalformedURLException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Paths;

public class ServerReader {

    public ServerReader() {
    }

    public byte[] read(String path) {
        try {
            return Files.readAllBytes(Paths.get(path));
        }
        catch (Exception ex) {
            return null;
        }
    }
}
