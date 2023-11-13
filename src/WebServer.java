import java.io.*;
import java.net.Socket;
import java.net.ServerSocket;
import java.net.InetAddress;
import java.nio.charset.StandardCharsets;
import java.security.KeyStore;
import java.security.cert.Certificate;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.text.MessageFormat;
import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.function.Function;
import java.util.function.Supplier;

import javax.net.ssl.*;
import javax.net.ssl.SSLServerSocket;

public class WebServer extends Thread {
    private static final String imgPattern = "GET /Images/";
    private static final String stylePattern = "GET /Styles/";
    private static final String scriptPattern = "GET /Scripts/";
    private static final String dataGetPattern = "GET /data";
    private static final String dataPostPattern = "POST /data";

    private final int port;
    private final String ip;
    private final String keyStorePassword;
    private final ThreadPoolExecutor pool;
    private final PrintStream out;
    private final ServerReader reader;
    private SSLContext sslContext;
    private SSLServerSocket server;
    private boolean working;


    private Supplier<byte[]> dataGetting;
    private Function<String, byte[]> dataParametrizedGetting;

    public WebServer(int port, String keyStorePassword) {
        this("127.0.0.1", port, keyStorePassword);
    }

    public  WebServer(String ip, int port, String keyStorePassword) {
        this.ip = ip;
        this.port = port;
        this.keyStorePassword = keyStorePassword;
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
            // Параметры пути к файлу PFX и пароля
            String pfxFilePath = "server.pfx";
            String pfxPassword = "your_password";

            // Загрузка PFX-файла
            KeyStore keyStore = KeyStore.getInstance("PKCS12");
            FileInputStream pfxInputStream = new FileInputStream(pfxFilePath);
            keyStore.load(pfxInputStream, pfxPassword.toCharArray());

            // Создание фабрики ключей
            KeyManagerFactory keyManagerFactory = KeyManagerFactory.getInstance("SunX509");
            keyManagerFactory.init(keyStore, pfxPassword.toCharArray());

            // Создание фабрики траст-менеджеров
            TrustManagerFactory trustManagerFactory = TrustManagerFactory.getInstance("SunX509");
            trustManagerFactory.init(keyStore);

            // Создание SSL контекста
            SSLContext sslContext = SSLContext.getInstance("TLSv1.2");
            sslContext.init(keyManagerFactory.getKeyManagers(), trustManagerFactory.getTrustManagers(), null);

            // Создание SSLServerSocketFactory
            SSLServerSocketFactory sslServerSocketFactory = sslContext.getServerSocketFactory();

            // Создание SSLServerSocket
            server = (SSLServerSocket) sslServerSocketFactory.
                            createServerSocket(port, 16, InetAddress.getByName("localhost"));

            out.println(MessageFormat.format("Web server start listening at https:\\\\{0}:{1}",
                                            server.getInetAddress().getHostAddress(),
                                            server.getLocalPort()));
        }
        catch (Exception ex) {
            out.println(MessageFormat.format("Error on creating socket on {0}:{1}. Message: {2}", ip, port, ex.getMessage()));
            result = false;
        }
        return result;
    }

    private void handling() {
        out.println("Start handling");
        while(working) {
            try {
                SSLSocket client = (SSLSocket) server.accept();
                pool.submit(() -> handleClient(client));
            }
            catch (IOException ex) {
                out.println(MessageFormat.format("Error while accepting clients. Exception: {0}",
                                                    ex.getMessage()));
            }
        }
    }

    private void handleClient(SSLSocket client) {
            out.println(MessageFormat.format(
                    "Client [{0}:{1}] accepting", client.getInetAddress().getHostAddress(),
                    client.getLocalPort()
            ));

            String request = getRequest(client);
            if (request != null) {
                ByteArrayOutputStream stream = new ByteArrayOutputStream();
                String response = getResponse(request, stream);
                sendResponse(client, response, stream.toByteArray());
            }
            closeClient(client);
    }

    private String getRequest(SSLSocket client) {
        String request = null;
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(client.getInputStream(), "UTF-8"));
            StringBuilder requestBuilder = new StringBuilder();
            String line;
            while (!(line = reader.readLine().trim()).equals("")) {
                requestBuilder.append(line);
                break;
            }

            request = requestBuilder.toString().split("\n")[0].replace("null", "");

            out.println(MessageFormat.format(
                    "Request: {0}", request));
        }
        catch (IOException ex) {
            out.println(MessageFormat.format("Error while getting request from client. Exception: {0}",
                                            ex.getMessage()));
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

        if (request.contains("GET / HTTP/1.1") || request.equals("")) {
            content = reader.read("Views/Index.html");
            contentType = "Content-Type: text/html; charset=utf-8\n";
        }
        else if (request.contains(imgPattern) ||
                request.contains(stylePattern) ||
                request.contains(scriptPattern)) {
            ByteArrayOutputStream contentStream = new ByteArrayOutputStream();
            contentType = getStaticFilesResponse(request, contentStream);
            if (contentType.equals("")) {
                httpOk = false;
            }
            else {
                content = contentStream.toByteArray();
            }
        }
        else if (request.contains(dataGetPattern)) {
            contentType = "Content-Type: text/json\n";
            content = dataGetting.get();
        }
        else if (request.contains(dataPostPattern)) {
            contentType = "Content-Disposition: attachment; filename=result.txt\n" +
                          "Content-Type: application/octet-stream\n";
            String parameters = request.split("POST /data?")[1].split(" HTTP/1.1")[0];
            content = dataParametrizedGetting.apply(parameters);
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
        //header.append("Date:").append(LocalDateTime.now()).append("\n");
        header.append(MessageFormat.format("Content-Length: {0}\n", content.length));
        header.append("Connection:close\n\n");

        try {
            outputStream.write(content);
        }
        catch (Exception ex) {
            return  "HTTP/1.1 400 Bad Request";
        }

        return header.toString();
    }

    private String getStaticFilesResponse(String request, ByteArrayOutputStream contentStream) {
        String contentType = "";
        if (request.contains(imgPattern)) {
            contentType = "Content-Type: image/jpeg\n";
        }
        else if (request.contains(stylePattern)) {
            contentType = "Content-Type: text/css\n";
        }
        else if (request.contains(scriptPattern)){
            contentType = "Content-Type: text/script\n";
        }
        String path = request.split("GET /")[1].split(" HTTP/1.1")[0];
        try {
            contentStream.write(reader.read("Views/" + path));
            contentStream.close();
        } catch (IOException ex) {
            return "";
        }
        return  contentType;
    }

    private boolean sendResponse(SSLSocket client, String header, byte[] content) {
        boolean result = true;
        try {
            out.println("Header " + header.length() + " Content " + content.length);
            OutputStream outputStream = client.getOutputStream();
            header = header.replace("\u00a0","");

            //ByteArrayOutputStream bytes = new ByteArrayOutputStream();
//            outputStream.write(header.getBytes(StandardCharsets.UTF_8));
//            outputStream.write(content);

            outputStream.write(header.getBytes(StandardCharsets.UTF_8));
            outputStream.write(content);
            outputStream.flush();
        }
        catch (IOException ex) {
            result = false;
            out.println(MessageFormat.format("Error while sending response: {0}", ex.getMessage()));
        }
        return result;
    }

    private void closeClient(SSLSocket client) {
        try {
            if (!client.isClosed()) {
                client.close();
            }
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
