import com.sun.net.httpserver.HttpContext;
import com.sun.net.httpserver.HttpServer;

import javax.websocket.ContainerProvider;
import javax.websocket.DeploymentException;
import javax.websocket.WebSocketContainer;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.URI;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Executors;

public class Server extends Thread {
    private String addres;
    private int port;
    private HttpServer server;
    private HttpContext context;

    public Server(String address, int port) throws IOException {
        this.addres = address;
        this.port = port;
        server = HttpServer.create(new InetSocketAddress(address, port), 16);
        context = server.createContext("/");
        context.setHandler(new Handler(System.out));
    }

    @Override
    public void run() {
        server.setExecutor(Executors.newCachedThreadPool());
        server.start();
    }
}
