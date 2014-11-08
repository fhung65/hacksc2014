package myotank;

import java.net.ServerSocket;
import java.net.Socket;
import java.io.IOException;
import java.io.OutputStream;
import java.io.BufferedInputStream;

public class TankRunnable implements Runnable
{
	private DataManager m_dataManager;
	private static final int REFRESH_RATE = 1000;

	public TankRunnable(DataManager dataManager)
	{
		m_dataManager = dataManager;
	}
	
	public void run()
	{
		while (true)
		{
			Socket client = getClient();
			if (client == null)
			{
				try {
					Thread.sleep(500);
				}
				catch (InterruptedException e)
				{ }
				continue;
			}
			
			OutputStream out = null;
			try {
				out = client.getOutputStream();
			}
			catch (IOException e) {
				continue;
			}
			BufferedInputStream in = null;
			try {
				in = new BufferedInputStream(client.getInputStream());
			}
			catch (IOException e) {
				try {
					out.close();
				}
				catch (IOException e2)
				{ }
				continue;
			}
			
			Thread outThread = new Thread(new TankOutRunnable(m_dataManager, out));
			Thread inThread = new Thread(new TankInRunnable(m_dataManager, in));
			
			outThread.start();
			inThread.start();
			
			try {
				outThread.join();
				inThread.join();
			}
			catch (InterruptedException e)
			{ }
				
			m_dataManager.setTankThreadsKilled(false);
		}
	}
	
	private Socket getClient()
	{
		ServerSocket socket = null;
		try {
			socket = new ServerSocket(Server.TANK_PORT);
		}
		catch (IOException e) {
			return null;
		}
		
		while (true)
		{
			try {
				Socket client = socket.accept();
				return client;
			}
			catch (IOException e) {
				continue;
			}
		}
	}
}