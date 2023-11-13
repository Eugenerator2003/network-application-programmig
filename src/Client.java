import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;

public class Client {
    public static void main(String[] args) {
        try {
            SSLSocket socket = (SSLSocket) SSLSocketFactory.getDefault().createSocket("127.0.0.1", 443);
            socket.startHandshake();
            OutputStream stream = socket.getOutputStream();
            stream.write("Hello world!".getBytes(StandardCharsets.UTF_8));
            stream.close();
            socket.close();
        }
        catch (IOException ex){
            System.out.println(ex.getMessage());
        }
    }
}
