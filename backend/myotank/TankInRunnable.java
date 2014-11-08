package myotank;

import java.io.InputStream;
import java.io.IOException;

public class TankInRunnable implements Runnable
{
	private DataManager m_dataManager;
	private InputStream m_in;
	
	public TankInRunnable(DataManager dataManager, InputStream in)
	{
		m_dataManager = dataManager;
		m_in = in;
	}
	
	public void run()
	{
		while (!m_dataManager.tankThreadsKilled())
		{
			byte[] data = new byte[16];
			try {
				m_in.read(data, 0, data.length);
			}
			catch (IOException e) {
				m_dataManager.setTankThreadsKilled(true);
			}
			/// TODO: Do something useful with the data
			System.out.println(data);
		}
	}
}