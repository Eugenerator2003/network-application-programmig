import java.io.ByteArrayOutputStream;
import java.nio.charset.StandardCharsets;
import java.text.MessageFormat;
import java.util.Arrays;
import java.util.stream.*;
import java.util.regex.Pattern;

public class Main {
    private static int vertices = 10;
    private static Edge[] edges = new Edge[] {
            new Edge(0, 1,  5),
            new Edge(0, 2,  3),
            new Edge(1, 2,  2),
            new Edge(1, 3,  7),
            new Edge(2, 3,  4),
            new Edge(3, 4,  1),
            new Edge(4, 0,  2),
            new Edge(0, 5,  3),
            new Edge(5, 2,  6),
            new Edge(4, 5,  8),
            new Edge(6, 7,  2),
            new Edge(7, 8,  4),
            new Edge(8, 6,  6),
            new Edge(6, 9,  5),
            new Edge(7, 9,  1),
            new Edge(9, 8,  3),
            new Edge(9, 5,  9),
            new Edge(2, 7,  5),
            new Edge(1, 4,  3),
            new Edge(3, 6,  4),
            new Edge(8, 5,  2),
            new Edge(2, 4,  1),
            new Edge(7, 5,  3),
            new Edge(9, 0,  7)
    };

    private static int getMinDistance(int source, int destination)
    {
        int result = 0;
        if (source < 0 || source > vertices || destination < 0 || destination > vertices)
        {
            result = 1;
        }

        int[] dists = Edge.distanceBellmanFord(edges, vertices, source);
        result = dists[destination - 1];
        return result;
    }


    private static byte[] getData() {
        StringBuilder builder = new StringBuilder("{ \"data\": [");
        for(int i = 0; i < edges.length; i++) {
            builder.append(edges[i].toString());
            if (i + 1 != edges.length) {
                builder.append(",");
            }
        }
        builder.append("]}");
        return builder.toString().getBytes(StandardCharsets.UTF_8);
    }

    private static byte[] getParametrizedData(String requestParameters) {
        byte[] data = new byte[0];
        String[] parameters = requestParameters.split("&");
        if (Arrays.stream(parameters).anyMatch(s -> s.contains("source=")) &&
                Arrays.stream(parameters).anyMatch(s -> s.contains("destination="))) {
            String source = (String) Arrays.stream(parameters).filter(s -> s.contains("source=")).toArray()[0];
            String destination = (String) Arrays.stream(parameters).filter(s -> s.contains("destination=")).toArray()[0];

            int sourceInt = Integer.parseInt(source.split("=")[1]);
            int destinationInt = Integer.parseInt(destination.split("=")[1]);

            int result = getMinDistance(sourceInt, destinationInt);
            String resultString = MessageFormat.format("Кратчайший путь из {0} в {1} равен {2}",
                                                        sourceInt, destinationInt, result);
            data = resultString.getBytes(StandardCharsets.UTF_8);
        }
        else {
            data = "Bad parameters".getBytes(StandardCharsets.UTF_8);
        }
        return data;
    }

    public static void main(String[] args) {
        WebServer server = new WebServer(8080);

        server.setDataGetting(Main::getData);
        server.setDataParametrizedGetting(Main::getParametrizedData);

        server.run();
        while (true);
    }
}