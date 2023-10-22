import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.text.MessageFormat;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.function.BiFunction;
import java.util.function.Consumer;
import java.util.function.Function;
import java.util.function.Supplier;

public class WebServer extends Thread {
    private static final String imgPattern = "Images/";
    private static final String stylePattern = "Styles/";
    private static final String scriptPattern = "Scripts/";

    private final int port;
    private final String ip;
    private final ThreadPoolExecutor pool;
    private final PrintStream out;
    private final ServerReader reader;
    private ServerSocket server;
    private boolean working;

    private Supplier<byte[]> dataGetting;
    private Function<String, byte[]> dataParametrizedGetting;

    public WebServer(int port) {
        this("127.0.0.1", port);
    }

    public  WebServer(String ip, int port) {
        this.ip = ip;
        this.port = port;
        out = System.out;
        reader = new ServerReader();
        pool = (ThreadPoolExecutor) Executors.newCachedThreadPool();
    }


    public void run() {
        if (!createSocket()) return;
        working = true;
        handling();
    }

    public void end() {
        working = false;
        try {
            server.close();
        }
        catch (IOException ex) {

        }
    }

    private boolean createSocket() {
        boolean result = true;
        try {
            server = new ServerSocket(port, 16, InetAddress.getByName(ip));
            out.println(MessageFormat.format("Web server start listening at {0}:{1}", ip, port));
        }
        catch (IOException ex) {
            out.println(MessageFormat.format("Error on creating socket on {0}:{1}", ip, port));
            result = false;
        }
        return result;
    }

    private void handling() {
        out.println("Start handling");
        while(working) {
            try {
                Socket client = server.accept();
                pool.submit(() -> handleClient(client));
            }
            catch (IOException ex) {
                out.println(MessageFormat.format("Error while handling clients. Exception: {0}",
                                                    ex.getMessage()));
            }
        }
    }

    private void handleClient(Socket client) {
        out.println(MessageFormat.format(
                "Client [{0}:{1}] accepting", client.getInetAddress().getHostAddress(),
                                                    client.getLocalPort()
        ));

        String request = getRequest(client);
        if (request != null)
        {
            ByteArrayOutputStream stream = new ByteArrayOutputStream();
            String response = getResponse(request, stream);
            sendResponse(client, response, stream.toByteArray());
        }
        closeClient(client);
    }

    private String getRequest(Socket client) {
        String request = null;
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(client.getInputStream()));
            while (!reader.ready());
            request = reader.readLine();
            out.println(MessageFormat.format(
                    "Request: {0}", request
            ));
        }
        catch (IOException ex) {
            out.println("Error while getting request from client");
        }
        return  request;
    }

    /**
     * @param outputStream - поток байтов для вывода
     * @return - строка ответа
     */
    private String getResponse(String request, ByteArrayOutputStream outputStream) {
        StringBuilder header = new StringBuilder();
        byte[] content = null;
        boolean httpOk = true;
        String contentType = "";

        if (request.contains("GET / HTTP/1.1")) {
            content = reader.read("Views/Index.html");
            contentType = "Content-Type: text/html; charset=utf-8\n";
        }
        else if (request.contains(imgPattern) ||
                request.contains(stylePattern) ||
                request.contains(scriptPattern)) {
            String path = request.split("GET /")[1].split(" HTTP/1.1")[0];
            content = reader.read("Views/" + path);
            if (request.contains(imgPattern)) {
                contentType = "Content-Type: image/jpeg\n";
            }
            else if (request.contains(stylePattern)) {
                contentType = "Content-Type: text/css\n";
            }
            else if (request.contains(scriptPattern)){
                contentType = "Content-Type: text/script\n";
            }
        }
        else if (request.contains("/data")) {
            if (request.contains("/data?")) {
                contentType = "Content-Disposition: attachment; filename=result.txt\n" +
                              "Content-Type: application/octet-stream\n";
                String parameters = request.split("GET /data?")[1].split(" HTTP/1.1")[0];
                content = dataParametrizedGetting.apply(parameters);
            }
            else {
                contentType = "Content-Type: text/json\n";
                content = dataGetting.get();
            }
        }
        else {
            httpOk = false;
        }

        if (httpOk) {
            header.append("HTTP/1.1 200 OK\n");
            header.append(contentType);
        }
        else {
            header.append("HTTP/1.1 404 Page Not Found");
            header.append("Content-Type: text/html; charset=utf-8\n");
            content = reader.read("Views/NotFound.html");
        }
        header.append(MessageFormat.format("Content-Length: {0}\n\n", content.length));

        try {
            outputStream.write(content);
        }
        catch (Exception ex) {
            return  "HTTP/1.1 400 Bad Request";
        }

        return header.toString();
    }

    private boolean sendResponse(Socket client, String header, byte[] content) {
        boolean result = true;
        try {
            out.println("Header " + header.length() + " Content " + content.length);
            OutputStream outputStream = client.getOutputStream();

            outputStream.write(header.getBytes(StandardCharsets.UTF_8));
            outputStream.write(content);
            outputStream.flush();
        }
        catch (IOException ex) {
            result = false;
        }
        return result;
    }

    private void closeClient(Socket client) {
        try {
            client.close();
        }
        catch (IOException ex) {
            out.println("Error while closing connection to client");
        }
    }

    public void setDataGetting(Supplier<byte[]> dataGetting) {
        this.dataGetting = dataGetting;
    }

    public void setDataParametrizedGetting(Function<String, byte[]> dataParametrizedGetting) {
        this.dataParametrizedGetting = dataParametrizedGetting;
    }
}
