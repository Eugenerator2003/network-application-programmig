import java.io.IOException;
import java.util.HashSet;
import java.util.Set;

import javax.websocket.*;
import javax.websocket.server.ServerEndpoint;

@ServerEndpoint("/websocket")
public class WebSocketController {
    private static int nextId;
    private static Set<Session> clients = new HashSet<Session>();

    @OnMessage
    public void onMessage(String message, Session session) {
        synchronized (clients) {
            for (Session client :
                    clients) {
                if (!client.equals(session)) {
                    try {
                        client.getBasicRemote().sendText(message);
                    } catch (IOException ex) {
                        System.out.println("Error while sending message to client");
                    }
                }
            }
        }

    }

    @OnOpen
    public void onOpen(Session session) {
    // Add session to the connected sessions set
        session.getUserProperties().put("username", "user_"+nextId++);
        synchronized(clients) {
            clients.add(session);
        }
    }

    @OnClose
    public void onClose (Session session) {
        synchronized(clients) {
            clients.remove(session);
        }
        System.out.println("client disconnected: " +
                session.getUserProperties().get("username"));
    }
    @OnError
    public void onError(Session session, Throwable thr) {
        System.out.println("error socket, maybe someone disconnected");
    }

}
