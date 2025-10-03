import java.io.*;
import java.net.*;
import javax.script.*;

public class TCPServer {
    public static void main(String[] args) throws IOException {
        ServerSocket serverSocket = new ServerSocket(5000);
        System.out.println("Server started. Waiting for client...");

        Socket socket = serverSocket.accept();
        System.out.println("Client connected.");

        BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        PrintWriter out = new PrintWriter(socket.getOutputStream(), true);

        while (true) {
            String command = in.readLine();
            if (command == null) break;

            switch (command) {
                case "HELLO":
                    out.println("Hello from Server!");
                    break;

                case "FILE":
                    try {
                        // Client sends file size first
                        String sizeLine = in.readLine();
                        long fileSize = Long.parseLong(sizeLine);

                        FileOutputStream fos = new FileOutputStream("received.txt");
                        InputStream is = socket.getInputStream();

                        byte[] buffer = new byte[4096];
                        long totalRead = 0;
                        int bytesRead;

                        while (totalRead < fileSize && (bytesRead = is.read(buffer)) != -1) {
                            fos.write(buffer, 0, bytesRead);
                            totalRead += bytesRead;
                        }

                        fos.close();
                        out.println("File received (" + totalRead + " bytes).");
                        System.out.println("File received successfully.");
                    } catch (Exception e) {
                        out.println("File transfer failed.");
                        e.printStackTrace();
                    }
                    break;

                case "CALC":
                    String expression = in.readLine();
                    try {
                        double result = eval(expression);
                        out.println("Result: " + result);
                    } catch (Exception e) {
                        out.println("Error in expression.");
                    }
                    break;

                default:
                    out.println("Unknown command.");
            }
        }

        socket.close();
        serverSocket.close();
    }

    public static double eval(String expr) throws Exception {
        ScriptEngine engine = new ScriptEngineManager().getEngineByName("JavaScript");
        if (engine == null) throw new RuntimeException("JavaScript engine not available.");
        Object result = engine.eval(expr);
        return Double.parseDouble(result.toString());
    }
}
