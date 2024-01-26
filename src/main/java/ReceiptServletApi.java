import com.example.laba8.DL.Models.Receipt;
import com.example.laba8.DL.Models.ReceiptType;
import com.example.laba8.DL.MySqlRepositoryProvider;
import com.example.laba8.DL.Persistance.JsonSerializer;
import com.example.laba8.DL.Persistance.Repository;
import jakarta.json.JsonObject;
import jakarta.servlet.ServletConfig;
import jakarta.servlet.ServletException;
import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;

import java.io.IOException;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.util.stream.Collectors;

@WebServlet("/receipts")
public class ReceiptServletApi extends HttpServlet {
    private JsonSerializer serializer;
    private Repository<Receipt> receiptRepository;
    private boolean repositoryConfigure;
    private String repositoryExceptionMessage;

    private PrintStream out = System.out;

    public void init(ServletConfig config) {
        serializer = new JsonSerializer();
        try {
            receiptRepository = MySqlRepositoryProvider.getReceiptRepository();
            repositoryConfigure = true;
        }
        catch (Exception ex){
            repositoryExceptionMessage = ex.getMessage();
        }
    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        out.println("Do get");
        if (repositoryConfigure) {
            response.setContentType("text/json");
            PrintWriter writer = response.getWriter();
            Object idObj = request.getParameter("id");
            JsonObject obj = null;
            if (idObj != null) {
                obj = serializer.serialize(receiptRepository.get(Long.valueOf(idObj.toString())));
            }
            else {
                obj = serializer.serialize(receiptRepository.getAll());
            }
            writer.println(obj.toString());
            writer.close();
        }
        else {
            sendRepositoryProblem(response);
        }
    }

    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws IOException {
        out.println("Do post");
        if (repositoryConfigure) {
            Long id = -1l;
            try {
                Receipt receipt = serializer.deserialize(request.getReader().lines().collect(Collectors.joining()));
                id = receiptRepository.create(receipt);
            }
            catch (Exception ex) {

            }
            response.setContentType("text/json");
            PrintWriter writer = response.getWriter();
            writer.println("{\"id\" : " + id.toString() + "}");
            writer.close();
        }
        else {
            sendRepositoryProblem(response);
        }
    }

    protected void doPut(HttpServletRequest request, HttpServletResponse response) throws IOException {
        out.println("Do put");
        if (repositoryConfigure) {
            Boolean result = true;
            try {
                Receipt receipt = serializer.deserialize(request.getReader().lines().collect(Collectors.joining()));
                receiptRepository.update(receipt);
            }
            catch (Exception ex) {
                result = false;
            }
            response.setContentType("text/json");
            PrintWriter writer = response.getWriter();
            writer.println("{\"result\" : \"" + result.toString() + "\"}");
            writer.close();
        }
        else {
            sendRepositoryProblem(response);
        }
    }

    protected void doDelete(HttpServletRequest request, HttpServletResponse response) throws IOException {
        out.println("Do delete");
        if (repositoryConfigure) {
            Boolean result = true;
            try {
                Object idObj = request.getParameter("id");
                Long id = Long.valueOf(idObj.toString());
                receiptRepository.remove(id);
            }
            catch (Exception ex) {
                result = false;
            }
            PrintWriter writer = response.getWriter();
            response.setContentType("text/json");
            writer.println("{\"result\" : \"" + result.toString() + "\"}");
            writer.close();
        }
        else {
            sendRepositoryProblem(response);
        }
    }

    protected void sendRepositoryProblem(HttpServletResponse response) throws IOException {
        response.setContentType("text/plain");
        PrintWriter writer = response.getWriter();
        writer.println(repositoryExceptionMessage);
        writer.close();
    }
}
