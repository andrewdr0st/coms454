import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class DataServer {
    private static int replica;
    private static int port;
    private static int primaryPort;
    private static boolean isPrimary;
    private static ServerSocket socket;
    private static ArrayList<Integer> backupList;

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
            System.exit(1);
		}
        String s = isPrimary ? "Primary server" : "Backup server";
        System.out.println(s + " listening on port " + port);

        if (isPrimary) {
            backupList = new ArrayList<>();
        } else {
            //join primary server
            try {
			    Socket joinSocket = new Socket("localhost", primaryPort);
                InputStream input = joinSocket.getInputStream();
                BufferedReader reader = new BufferedReader(new InputStreamReader(input));
                OutputStream output = joinSocket.getOutputStream();
			    PrintWriter writer= new PrintWriter(output,true);

                writer.println("JOIN:" + port);
                String line = reader.readLine();
                System.out.println("Got response: " + line);

                joinSocket.close();
            } catch (Exception e) {
                System.out.println("Could not connect to primary server");
                System.exit(1);
            }
        }

        //main server loop
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
                if (isPrimary) {
                    writeValue(value);
                } else {
                    sendUpdateToPrimary(value);
                }
                writer.println("Write complete");
            } else if (request.equals("UPDATE")) {
                if (isPrimary) {
                    writeValue(value);
                } else {
                    replica = value;
                }
                writer.println("ack");
            } else if (request.equals("JOIN") && isPrimary) {
                backupList.add(value);
                System.out.println("Backup server at port " + value + " has joined");
                writer.println("ack");
            } else {
                System.out.println("Recieved an invalid request");
            }

            client.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static synchronized void writeValue(int value) {
        replica = value; //update replica syncronously

        if (!isPrimary) {
            return;
        }

        ExecutorService pool = Executors.newFixedThreadPool(backupList.size());
        CountDownLatch latch = new CountDownLatch(backupList.size());

        //send update to backup servers
        //idea for using countdown latch provided by AI assistance
        for (int port : backupList) {
            pool.submit(() -> { //spawn a thread to communicate with each backup
                try {
                    updateBackup(port, value);
                } catch (Exception e) {
                    System.out.println("Could not communicate with server at port " + port);
                } finally {
                    latch.countDown();
                }
            });
        }

        //wait for all threads to finish
        try {
            latch.await();
        } catch (Exception e) {
            System.out.println("Thread interupted. Shutting down.");
            System.exit(1);
        }
        pool.shutdown();
    }

    private static void updateBackup(int port, int value) throws IOException {
        Socket backupSocket = new Socket("localhost", port);
        InputStream input = backupSocket.getInputStream();
        BufferedReader reader = new BufferedReader(new InputStreamReader(input));
        OutputStream output = backupSocket.getOutputStream();
		PrintWriter writer = new PrintWriter(output,true);

        //send updated value to backup servers and wait for ack
        writer.println("UPDATE:" + value);
        reader.readLine();
        backupSocket.close();
    }

    private static void sendUpdateToPrimary(int value) throws IOException {
        Socket priamrySocket = new Socket("localhost", primaryPort);
        InputStream input = priamrySocket.getInputStream();
        BufferedReader reader = new BufferedReader(new InputStreamReader(input));
        OutputStream output = priamrySocket.getOutputStream();
        PrintWriter writer = new PrintWriter(output,true);

        //ask primary to update and wait for ack
        writer.println("UPDATE:" + value);
        reader.readLine();
        priamrySocket.close();
    }
}