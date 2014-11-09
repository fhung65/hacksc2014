package myotank;

public class Server
{
	public final static int TANK_PORT = 1420;
	public final static int CLIENT_PORT = 2420;

	public static void main(String[] args)
	{
		DataManager dataManager = new DataManager();
		Thread tankThread = new Thread(new TankRunnable(dataManager));
		Thread clientThread = new Thread(new ClientRunnable(dataManager));
		
		tankThread.start();
		clientThread.start();
	}
}