import java.io.*;
import java.net.*;

public class TCPClient {
    public static void main(String[] args) throws IOException {
        Socket socket = new Socket("10.10.13.17", 5000);

        BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        PrintWriter out = new PrintWriter(socket.getOutputStream(), true);

        BufferedReader console = new BufferedReader(new InputStreamReader(System.in));

        System.out.println("Connected to server. Type commands: HELLO, FILE, CALC, or EXIT");

        while (true) {
            System.out.print("> ");
            String command = console.readLine();

            if (command.equalsIgnoreCase("EXIT")) {
                break;
            }

            out.println(command);

            switch (command) {
                case "HELLO":
                    System.out.println("Server: " + in.readLine());
                    break;

                case "FILE":
                    System.out.print("Enter file path to send: ");
                    String filePath = console.readLine();
                    File file = new File(filePath);

                    if (!file.exists()) {
                        System.out.println("File not found!");
                        break;
                    }

                    // Send file size first
                    out.println(file.length());

                    FileInputStream fis = new FileInputStream(file);
                    OutputStream os = socket.getOutputStream();

                    byte[] buffer = new byte[4096];
                    int bytesRead;
                    while ((bytesRead = fis.read(buffer)) != -1) {
                        os.write(buffer, 0, bytesRead);
                    }
                    os.flush();
                    fis.close();

                    System.out.println("Server: " + in.readLine());
                    break;

                case "CALC":
                    System.out.print("Enter math expression (e.g. 3+5*2): ");
                    String expr = console.readLine();
                    out.println(expr);
                    System.out.println("Server: " + in.readLine());
                    break;

                default:
                    System.out.println("Server: " + in.readLine());
            }
        }

        socket.close();
    }
}
