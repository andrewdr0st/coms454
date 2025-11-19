import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;
import java.util.concurrent.locks.ReentrantLock;

public class DataServer {

    private static int replica;
    private static int port;
    private static int primaryPort;
    private static boolean isPrimary;
    private static ServerSocket socket;
     private final ReentrantLock lock = new ReentrantLock(true);

    public static void main(String[] args) {
        try {
            if (args.length > 1) { //become backup server
                port = Integer.parseInt(args[0]);
                primaryPort = Integer.parseInt(args[1]);
                isPrimary = false;
            } else if (args.length == 1) { //become primary server
                port = Integer.parseInt(args[0]);
                isPrimary = true;
            } else {
                throw new RuntimeException();
            }
        } catch(Exception e) {
            System.out.print("Invalid command. Proper usage:\njava DataServer int (int)\n");
            System.exit(1);
        }
        replica = 0;
        try {
            socket = new ServerSocket(port);
        } catch (IOException e) {
			e.printStackTrace();
		}
        String s = isPrimary ? "Primary server" : "Backup server";
        System.out.println(s + " listening on port " + port);

        while (true) {
            try {
                Socket client = socket.accept();
                new Thread(() -> handleMessage(client)).start();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private static void handleMessage(Socket client) {
        try {
            InputStream input = client.getInputStream();
            BufferedReader reader = new BufferedReader(new InputStreamReader(input));
            OutputStream output = client.getOutputStream();
			PrintWriter writer = new PrintWriter(output,true);

            String[] s = reader.readLine().split(":");
            String request = s[0].toUpperCase();
            int value = s.length > 1 ? Integer.parseInt(s[1]) : 0;

            if (request.equals("READ")) {
                writer.println(replica);
            } else if (request.equals("WRITE")) {

            } else if (request.equals("UPDATE")) {

            } else if (request.equals("JOIN") && isPrimary) {

            } else {
                System.out.println("Recieved an invalid request");
            }

            client.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}