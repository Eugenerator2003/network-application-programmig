import java.text.MessageFormat;

public class Edge {

    public static int[] distanceBellmanFord(Edge[] edges, int numVertices, int source) {
        int[] distance = new int[edges.length];
        for (int i = 0; i < numVertices; i++)
            distance[i] = Integer.MAX_VALUE;

        source--;
        distance[source] = 0;

        // Релаксация рёбер
        for (int i = 1; i < numVertices; i++) {
            for (Edge edge : edges) {
                if (distance[edge.source] != Integer.MAX_VALUE &&
                        distance[edge.source] + edge.weight < distance[edge.destination]) {
                    distance[edge.destination] = distance[(edge.source)] + edge.weight;
                }
            }
        }

        // Проверка наличия циклов с отрицательным весом
        for (Edge edge : edges) {
            if (distance[edge.source] != Integer.MAX_VALUE &&
                    distance[edge.source] + edge.weight < distance[edge.destination]) {
                return new int[0];
            }
        }

        return distance;
    }

    private int source;
    private int destination;
    private int weight;

    public Edge(int source, int destination, int weight) {
        this.source = source;
        this.destination = destination;
        this.weight = weight;
    }

    public int getSource() {
        return source;
    }

    public void setSource(int source) {
        this.source = source;
    }

    public int getDestination() {
        return destination;
    }

    public void setDestination(int destination) {
        this.destination = destination;
    }

    public int getWeight() {
        return weight;
    }

    public void setWeight(int weight) {
        this.weight = weight;
    }

    @Override
    public String toString() {
        return "{" +
                MessageFormat.format("\"source\" : \"{0}\", \"destination\" : \"{1}\", \"weight\" : \"{2}\"",
                                                source, destination, weight) +
                "}";
    }
}
