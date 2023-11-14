import com.sun.net.httpserver.Headers;
import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

public class Handler implements HttpHandler {

    private PrintStream log;

    public Handler(PrintStream out) {
        this.log = out;
    }

    @Override
    public void handle(HttpExchange exchange) throws IOException {
        String method = exchange.getRequestMethod();
        String request = exchange.getRequestURI().toString();
        log.print(method);
        log.print(" ");
        log.println(request);
        Headers headers = exchange.getResponseHeaders();
        ByteArrayOutputStream response = new ByteArrayOutputStream();
        if (request.equals("/")) {
            response.write(Files.readAllBytes(Path.of("Views/Index.html")));
            headers.add("Content-type", "text/html; charset=utf-8");
        }
        else if (request.contains(".js")) {
            response.write(Files.readAllBytes(Path.of("Views" + request)));
            headers.add("Content-type", "text/javascript");
        }
        else if (request.contains(".css")) {
            response.write(Files.readAllBytes(Path.of("Views" + request)));
            headers.add("Content-type", "text/css");
        }
        exchange.sendResponseHeaders(200, response.size());
        exchange.getResponseBody().write(response.toByteArray());
        exchange.getResponseBody().close();
    }
}
