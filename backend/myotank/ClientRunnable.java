package myotank;

import java.io.IOException;
import java.io.BufferedOutputStream;
import java.io.OutputStream;
import java.io.InputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class ClientRunnable implements Runnable
{
	private DataManager m_dataManager;

	public ClientRunnable(DataManager dataManager)
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
			
			//OutputStream out = null;
			BufferedOutputStream out = null;
			try {
				out = new BufferedOutputStream(client.getOutputStream());
				//out = client.getOutputStream();
			}
			catch (IOException e) {
				continue;
			}
			InputStream in = null;
			try {
				in = client.getInputStream();
			}
			catch (IOException e) {
				try {
					out.close();
				}
				catch (IOException e2)
				{ }
				continue;
			}
			
			Thread outThread = new Thread(new ClientOutRunnable(m_dataManager, out));
			Thread inThread = new Thread(new ClientInRunnable(m_dataManager, in));
			
			outThread.start();
			inThread.start();
			
			try {
				outThread.join();
				inThread.join();
			}
			catch (InterruptedException e)
			{ }
			
			try {
				out.close();
				in.close();
			}
			catch (IOException e)
			{ }
			m_dataManager.setClientThreadsKilled(false);
		}
	}
	
	private Socket getClient()
	{
		ServerSocket socket = null;
		try {
			socket = new ServerSocket(Server.CLIENT_PORT);
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