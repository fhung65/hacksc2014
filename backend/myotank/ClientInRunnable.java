package myotank;

import java.io.IOException;
import java.io.InputStream;

public class ClientInRunnable implements Runnable
{
	private DataManager m_dataManager;
	private InputStream m_in;

	public ClientInRunnable(DataManager dataManager, InputStream in)
	{
		m_dataManager = dataManager;
		m_in = in;
	}
	
	public void run()
	{
		while (!m_dataManager.clientThreadsKilled())
		{
			byte[] data = new byte[1];
			try {
				int bytesRead = m_in.read(data);
				if (bytesRead == -1)
					throw new IOException();
				m_dataManager.setMyoData(data[0]);
			}
			catch (IOException e) {
				m_dataManager.setClientThreadsKilled(true);
			}
		}
	}
}